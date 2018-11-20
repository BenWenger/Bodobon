import os
import subprocess
import filecmp
import shutil
import sys

os.chdir( os.path.dirname(os.path.realpath(__file__)) )

class Tester:
    modes = []
    
    def iterateModes(self):
        self.modes = [
            fn[:-4]
            for fn in os.listdir('../lua')
            if fn[-4:] == '.lua'
        ]
        
    def getFiles(self):
        return [
            fn[:-4]
            for fn in os.listdir('testdata')
            if fn[-4:] == '.asm' and fn.find('_') > 0
        ]
        
    def callAsm(self, filename):
        mode = filename[:filename.find('_')]
        subprocess.run(
            ['bodoasm', mode, 'testdata/'+filename+'.asm', 'tmpdir/'+filename+'.bin'],
            check=True,
            stdout=subprocess.DEVNULL
        )
        
    def compare(self, filename):
        srcfile = 'testdata/'+filename+'.bin'
        dstfile = 'tmpdir/'+filename+'.bin'
        return filecmp.cmp(srcfile, dstfile)
        
    def clean(self):
        config = 'Release_x86'
        if len(sys.argv) > 1:
            config = sys.argv[1]
        if os.path.isdir('tmpdir'):
            shutil.rmtree('tmpdir')
        if os.path.isdir('lua'):
            shutil.rmtree('lua')
        os.mkdir('tmpdir')
        shutil.copytree( '../lua', 'lua' )
        shutil.copyfile( '../bin/'+config+'/bodoasm.exe', 'bodoasm.exe')
        
    def runTests(self):
        self.clean()
        for test in self.getFiles():
            print((test + '...').ljust(40), end='')
            self.callAsm(test)
            if self.compare(test):
                print('OK')
            else:
                print('COMPARE FAILURE')
    

if __name__ == '__main__':
    test = Tester()
    print('----- Running tests ----- ')
    test.runTests()
    print('----- Tests complete ----- ')