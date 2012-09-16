#!/usr/bin/python
import glob
files = glob.glob('*.cpp') + glob.glob('*.mm')
print '// Unity (bulk compilation) unit file for Rush library'
print '// THIS FILE IS GENERATED AUTOMATICALLY -- DO NOT EDIT'
for f in files:
	print '#include "' + f + '"'
print '// eof'
