
#include "lexer.h"
#include "error.h"
#include "filenamepool.h"

namespace bodoasm
{
    
    Lexer::Lexer(ErrorReporter& er)
        : err(er)
    {}

    void Lexer::unget(const Token& t)
    {
        ungotten.push_back(t);
    }

    void Lexer::startFile(const std::string& filename)
    {
        auto file = std::make_unique<dshfs::FileStream>( filename );
        if(!file->good())
            err.fatal(nullptr, "Unable to open file '" + filename + "' for reading");
        
        cur.clear();
        cur.pos.fileId = FilenamePool::add(filename);
        cur.file = std::move(file);

        ungotten.clear();
        includeStack.clear();
    }
    
    void Lexer::startInclude(const std::string& filename, const Position* errpos)
    {
        auto file = std::make_unique<dshfs::FileStream>( filename );
        if(!file->good())
            err.error(errpos, "Unable to open file '" + filename + "' for reading");

        includeStack.emplace_back( std::move(cur) );
        cur.clear();
        cur.pos.fileId = FilenamePool::add(filename);
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

    auto Lexer::getNext() -> Token
    {
        Token out;
        //   TODO
        return out;
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
        if(eol())
            return false;
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
                        tok.type = Token::Type::InputEnd;
                    }
                    else
                    {
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
            while( onWhitespaceChar() )
                advance();

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
}

