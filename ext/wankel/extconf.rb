require 'mkmf'
require 'rbconfig'

$CFLAGS << '-Wall'

if ARGV.include?('--coverage')
  $CFLAGS << " -fprofile-arcs  -ftest-coverage"
  $DLDFLAGS << ' --coverage'
end

if have_library('yajl')
  create_makefile('wankel/wankel')
else
  puts "Couldn't find YAJL library"
end
