import json

def jsonWriteDict(file, dictionary, indent=2):
    file.write( bytearray(json.JSONEncoder(indent=indent).encode(dictionary), encoding='utf-8') )
    
def buildHeader(isHost):
    out = {
        '_version':     'No version'
    }
    if isHost:
        out['_fileType'] = 'Bodobeep host file'
    else:
        out['_fileType'] = 'Bodobeep song file'
    
    return out

class HostBuilder:
    def build(self, file):
    
        out = {
            '_header':      buildHeader(True),
            '_properties':  {
                '_driver':      'FinalFantasy.lua'
            }
        }
        
        # Load data tables!
        file.seek(0x37309)
        
        out['freqTable'] =      self._loadFreqTable(file)
        out['lengthTable'] =    self._loadLengthTable(file)
        out['speedTable'] =     self._loadSpeedTable(file)
        out['envTables'] =      self._loadEnvTables(file)
        
        return out
        
        
    def _loadFreqTable(self, file):
        out = []
        for i in range(12*4):   # 12 notes, 4 octaves
            v = file.read(2)
            out.append( v[0] | (v[1]<<8) )
        return out
        
    def _loadLengthTable(self, file):
        out = []
        for x in range(6 * 16): # 6 tempos, 16 lengths per tempo
            out.append( file.read(1)[0] )
        return out
        
    def _loadSpeedTable(self, file):
        out = []
        for i in range(16):     # 16 speeds
            out.append( file.read(2)[0] )
        return out
        
    def _loadEnvTables(self, file):
        out = []
        for x in range(16):     # 16 patterns
            pattern = []
            for i in range(16): # 32 entries per pattern
                pattern.append( file.read(1)[0] )
            out.append(pattern)
        return out

############################################        

def makeTone(pitch, length, env, envSpeed):
    return {
        'pitch':    pitch,
        'length':   length,
        'env':      env,
        'envSpeed': envSpeed
    }
    
############################################

class ChannelBuilder:
    
    def build(self, file, songId, chanId, chName):
        self._seekToChanStart(file, songId, chanId)
        
        loopCtr =       0
        curOctave =     0
        curEnv =        0
        curEnvSpeed =   0
        curTempo =      0
        score =         []
        loopPos =       -1
        
        hasFullStop =   False
        
        trekkedPositions = {}
        
        keepGoing = True
        i = 0
        while keepGoing:
            tl = file.tell()
            if tl not in trekkedPositions:
                trekkedPositions[ tl ] = len(score)
            
            # prevent infinite looping
            i += 1
            if i >= 100000:
                v = 0xFF        # force a full stop
            else:
                v = file.read(1)[0]
                
            # our big elif chain to parse the score
            if v < 0xC0:
                # standard note
                p = (v >> 4) + curOctave
                L = (v & 0x0F) + curTempo
                score.append( makeTone(p, L, curEnv, curEnvSpeed) )
            elif v < 0xD0:
                # rest / sustain
                L = (v & 0x0F) + curTempo
                score.append( makeTone(-1, L, curEnv, curEnvSpeed) )
            elif v == 0xD0:
                # Infinite loop marker -- possibly [likely] the end of the song
                self._readPtrAndSeek(file)
                pos = file.tell()
                if pos in trekkedPositions:     # and loopCtr == 0 ??
                    keepGoing = False
                    loopPos = trekkedPositions[pos]
            elif v < 0xD8:
                # counted loop
                if loopCtr == 0:
                    loopCtr = v & 7
                    self._readPtrAndSeek(file)
                elif loopCtr == 1:
                    loopCtr = 0
                else:
                    loopCtr -= 1
                    self._readPtrAndSeek(file)
            elif v < 0xDC:
                # octave select
                curOctave = (v & 0x03) * 12
            elif v < 0xE0:
                # unused
                pass
            elif v < 0xF0:
                # envelope select
                curEnv = v & 0x0F
            elif v < 0xF8:
                # unused
                pass
            elif v == 0xF8:
                # set envelope speed
                curEnvSpeed = file.read(1)[0] & 0x0F
            elif v < 0xFF:
                # Tempo select
                curTempo = (v - 0xF9) * 16
            else:
                hasFullStop = True
                keepGoing = False
        
        # done with the loop, finalize our stuff
        properties = {}
        if loopPos >= 0 and not hasFullStop:
            properties['_loopPos'] = loopPos
            
        return {
            '_score':       score,
            '_properties':  properties
        }
        
        
        
    def _seekToChanStart(self, file, songId, chanId):
        ptrPos = 0x34010 + (songId * 8) + (chanId * 2)
        file.seek(ptrPos)
        self._readPtrAndSeek(file)
        
    def _readPtrAndSeek(self, file):
        bts = file.read(2)
        ptr = bts[0] | (bts[1] << 8)
        file.seek((ptr & 0x3FFF) + 0x34010)
    
class SongBuilder:
    def build(self, file, songId):
        channels = [(0, 'pulse1'), (1, 'pulse2'), (2, 'triangle')]
        bldr = ChannelBuilder()
        return {
            '_header':      buildHeader(False),
            '_properties':  {
                '_host':        'FinalFantasy.bbh.json'
            },
            '_channels': {chName : bldr.build(file, songId, chId, chName)
                for (chId, chName) in channels}
        }
        
    
# 24 songs total

############################################
if __name__ == "__main__":
    srcFile = open("fforig.nes", "rb")
    
    hBldr = HostBuilder()
    sBldr = SongBuilder()
    
    outFile = open("FinalFantasy.bbh.json", "wb")
    jsonWriteDict( outFile, hBldr.build(srcFile) )
    outFile.close()
    
    for i in range(24):
        outFile = open("{:02X}_song.bbs.json".format(i), "wb")
        jsonWriteDict( outFile, sBldr.build(srcFile, i) )
        outFile.close()
