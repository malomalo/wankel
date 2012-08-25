require 'mkmf'
require 'rbconfig'

$CFLAGS << ' -Wall -funroll-loops'

if have_library('yajl')
  create_makefile('wankel/wankel')
else
  puts "Couldn't find YAJL library"
end
