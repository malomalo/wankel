Gem::Specification.new do |s|
  s.name        = 'wankel'
  s.version     = '0.6.2'
  s.licenses    = ['MIT']
  s.authors     = ['Jon Bracy']
  s.email       = ['jonbracy@gmail.com']
  s.homepage    = 'http://wankelrb.org'
  s.summary     = 'Streaming JSON parser and encoder'
  s.description = 'A JSON parser that enables streaming parsing and encoding of JSON'
  s.metadata    = {
    "issue_tracker" => "https://github.com/malomalo/wankel/issues"
  }
  
  s.extra_rdoc_files = %w(README.md)
  s.rdoc_options << '--title' << 'Wankel'
  s.rdoc_options << '--main' << 'README.md'
  s.rdoc_options << '--line-numbers'

  s.files = `git ls-files`.split("\n")
  s.extensions = ['ext/wankel/extconf.rb']
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.require_paths = ['lib']

  s.requirements << 'yajl, v2.1'
  
  s.required_ruby_version = '>= 2.0.0'

  s.add_development_dependency 'rake-compiler',       '~> 0.9'
  s.add_development_dependency 'minitest',            '~> 5.3'
  s.add_development_dependency 'minitest-reporters',  '~> 1.0'
  s.add_development_dependency 'mocha',               '~> 1.1'
  s.add_development_dependency 'sdoc',                '~> 0.4'
  s.add_development_dependency 'sdoc-templates-42floors', '~> 0.3'
end