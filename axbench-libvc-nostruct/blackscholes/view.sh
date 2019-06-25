#~/bin/bash
grep -v "Find" bin/test.log | grep -v "Skip" | grep -v "MetaData" | grep -v "function clone" | less
