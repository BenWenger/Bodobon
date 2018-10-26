
#include "lexer.h"
#include "error.h"
#include "stringpool.h"

namespace bodoasm
{
    
    Lexer::Lexer(ErrorReporter& er)
        : err(er)
    {}

    void Lexer::unget(const Token& t)
    {
        cur.ungotten.push_back(t);
    }

    void Lexer::startFile(const std::string& filename)
    {
        auto file = std::make_unique<dshfs::FileStream>( filename );
        if(!file->good())
            err.fatal(nullptr, "Unable to open file '" + filename + "' for reading");
        
        cur.clear();
        cur.pos.fileId = StringPool::toInt(filename);
        cur.file = std::move(file);

        cur.ungotten.clear();
        includeStack.clear();
    }
    
    void Lexer::startInclude(const std::string& filename, const Position* errpos)
    {
        auto file = std::make_unique<dshfs::FileStream>( filename );
        if(!file->good())
            err.error(errpos, "Unable to open file '" + filename + "' for reading");

        includeStack.emplace_back( std::move(cur) );
        cur.clear();
        cur.pos.fileId = StringPool::toInt(filename);
        cur.file = std::move(file);
    }
    
    bool Lexer::getNextLine()
    {
        if(!cur.file)
            return false;
        if(cur.file->eof())
            return false;
        std::getline( *cur.file, cur.text );
        cur.pos.lineNo++;
        cur.pos.linePos = 0;
        if(!cur.file->eof() && cur.file->fail())
            err.fatal(&cur.pos, "Unexpected error occurred when trying to read from file");
        return true;
    }

    inline bool Lexer::eol() const
    {
        return cur.pos.linePos >= cur.text.size();
    }

    inline char Lexer::peek() const
    {
        if(eol())   return ' ';
        return cur.text[cur.pos.linePos];
    }

    inline char Lexer::advance()
    {
        char out = peek();
        ++cur.pos.linePos;
        return out;
    }

    bool Lexer::onWhitespaceChar() const
    {
        if(eol())               // have to check EOL first, because peek will return whitespace if on EOL, but that
            return false;       //   technically isn't whitespace
        switch(peek())
        {
        case ' ':
        case '\t':
            return true;
        }
        return false;
    }

    bool Lexer::onSkippableChar() const
    {
        switch(peek())
        {
        case ' ':
        case ';':
        case '\t':
        case '\\':
            return true;
        }
        return false;
    }

    void Lexer::skipWhitespace()
    {
        while(onWhitespaceChar())
            advance();
    }

    // Return true if we reached a new token and it is ready to be lexed
    // Return false if we ran into interesting whitespace (end of file, end of command)
    //     in which case, 'tok' is populated with the token
    bool Lexer::skipToNextToken(Token& tok)
    {
        while(onSkippableChar())
        {
            // are we at EOL? 
            if(eol())
            {
                tok.pos = cur.pos;
                tok.type = Token::Type::CmdEnd;
                if(!getNextLine())                  // EOF reached.  If we have an include stack, go back on it
                {
                    if(includeStack.empty())
                    {
                        cur.clear();
                        tok.type = Token::Type::CmdEnd;
                    }
                    else
                    {
                        tok.type = Token::Type::FileEnd;
                        cur = std::move(includeStack.back());
                        includeStack.pop_back();
                    }
                }
                return false;
            }

            // same-line EOC?
            if(peek() == '\\')
            {
                tok.pos = cur.pos;
                tok.type = Token::Type::CmdEnd;
                advance();
                return false;
            }

            // skip over any whitespace
            skipWhitespace();

            // skip over any comments
            if(peek() == ';')
            {
                advance();
                if(advance() == '-')       // block comment
                {
                    while(true)
                    {
                        if(advance() == '-' && peek() == ';')
                        {
                            advance();
                            break;
                        }
                        if(eol() && !getNextLine())
                            err.error(&cur.pos,"Unexpected EOF reached in block comment");
                    }
                }
                else                    // line comment
                    cur.pos.linePos = cur.text.size();          // skip to EOL
            }
        }
        return true;
    }
    
    auto Lexer::getNext() -> Token
    {
        Token tok;
        
        // if anything is ungotten, return that
        if(!cur.ungotten.empty())
        {
            tok = cur.ungotten.back();
            cur.ungotten.pop_back();
            return tok;
        }


        if(!skipToNextToken(tok))
            return tok;

        tok.pos = cur.pos;

        char c = advance();
        switch(c)
        {
        case '!':   case '@':   case '#':   case '^':   case '&':   case '*':   case '(':   case ')':
        case '/':   case '[':   case ']':   case '{':   case '}':   case '<':   case '=':   case '>':
        case ':':   case '?':   case '~':   case '`':   case ',':   case '|':   case '-':   case '+':
        case '.':   case '$':   case '%':
            tok.str.clear();
            tok.str.push_back(c);
            tok.type = Token::Type::Operator;
            break;

        case '\'':  case '\"':              lexStringLiteral(tok, c);   break;

        default:
            if(isSymbolChar(c))             lexSymbol(tok, c);
            else
                err.error(&cur.pos, std::string("Unexpected character '") + c + "'");
            break;
        }

        tok.ws_after = onSkippableChar();
        return tok;
    }

    void Lexer::lexStringLiteral(Token& tok, char closer)
    {
        char c;
        while(true)
        {
            while(eol())
            {
                if(!getNextLine())
                    err.error(&cur.pos, "Unexpected EOF reached in string literal");
            }

            c = advance();
            if(c == closer)
                break;
            if(c == '\\')
            {
                bool escaped = true;
                switch(peek())
                {
                    // TODO hex input
                case '\\':  c = '\\';           break;
                case 'n':   c = '\n';           break;
                case 't':   c = '\t';           break;
                case 'r':   c = '\r';           break;
                case '\'':  c = '\'';           break;
                case '\"':  c = '\"';           break;
                default:
                    err.warning(&cur.pos, std::string("Unrecognized escape character (") + peek() + ") found in string literal");
                    escaped = false;
                    break;
                }
                if(escaped)
                {
                    tok.str.push_back(c);
                    advance();
                }
            }
            else
                tok.str.push_back(c);
        }

        // double-quote strings are null terminated
        tok.type = Token::Type::String;
        if(closer == '\"')
            tok.str.push_back('\0');
    }

    bool Lexer::isSymbolChar(char c)
    {
        if(c >= 'a' && c <= 'z')        return true;
        if(c >= 'A' && c <= 'Z')        return true;
        if(c == '_')                    return true;
        if(c >= '0' && c <= '9')        return true;
        return false;
    }

    void Lexer::lexSymbol(Token& tok, char c)
    {
        tok.type = Token::Type::Symbol;
        tok.str.clear();
        tok.str.push_back(c);
        while(isSymbolChar(c = peek()))
        {
            tok.str.push_back(c);
            advance();
        }
    }
}

