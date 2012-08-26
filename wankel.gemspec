Gem::Specification.new do |s|
  s.name = 'wankel'
  s.version = '0.0.1'
  s.authors = ['Jon Bracy']
  s.email = 'jon@42floors.com'
  s.extensions = ['ext/wankel/extconf.rb', 'ext/wankel/sax_parser/extconf.rb']
  s.files = `git ls-files`.split("\n")
  s.homepage = 'http://github.com/malomalo/wankel'
  s.require_paths = ['lib']

  s.summary = 'SAX based parsing for JSON'

  s.required_ruby_version = '>= 1.9.3'
end
