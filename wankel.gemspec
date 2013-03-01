Gem::Specification.new do |s|
  s.name = 'wankel'
  s.version = '1.0.0'
  s.authors = ['Jon Bracy']
  s.email = ['jonbracy@gmail.com']
  s.homepage = 'http://wankelrb.com'
  s.summary = 'SAX based parser for JSON'
  s.description = 'SAX based parser for JSON'
  
  s.rubyforge_project = "wankel"

  s.files = `git ls-files`.split("\n")
  s.extensions = ['ext/wankel/extconf.rb']
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.require_paths = ['lib']

  s.required_ruby_version = '>= 1.9.3'
  
  s.add_development_dependency 'rake-compiler'
  s.add_development_dependency 'minitest'
  s.add_development_dependency 'turn'
  s.add_development_dependency 'mocha'
end
