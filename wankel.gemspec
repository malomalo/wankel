Gem::Specification.new do |s|
  s.name        = 'wankel'
  s.version     = '0.6.1'
  s.licenses    = ['MIT']
  s.authors     = ['Jon Bracy']
  s.email       = ['jonbracy@gmail.com']
  s.homepage    = 'http://wankelrb.com'
  s.summary     = 'Streaming JSON parser and encoder'
  s.description = 'A JSON parser that enables streaming parsing and encoding of JSON'

  s.extra_rdoc_files = %w(README.md)
  s.rdoc_options.concat ['--main', 'README.md']

  s.files = `git ls-files`.split("\n")
  s.extensions = ['ext/wankel/extconf.rb']
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.require_paths = ['lib']

  s.required_ruby_version = '>= 2.0.0'

  s.add_development_dependency 'rake-compiler',       '~> 0.9'
  s.add_development_dependency 'minitest',            '~> 5.3'
  s.add_development_dependency 'minitest-reporters',  '~> 1.0'
  s.add_development_dependency 'mocha',               '~> 1.1'
  s.add_development_dependency 'sdoc',                '~> 0.4'
  s.add_development_dependency 'sdoc-templates-42floors', '~> 0.3'
end
