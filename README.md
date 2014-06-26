<a href="http://wanklerb.com">![Wankel](/logo.png)</a>

**Wankel** is a Ruby gem that provides C bindings to the
[YAJL 2](http://lloyd.github.io/yajl/) C Library.

Wankel provides gerneral parsing and encoding to and from
[JSON](http://json.org/), but also a SAX style parser and encoder for streaming
parsing and generation.

Features
--------

* SAX style JSON parsing and encoding
* JSON parsing and encoding directly to and from an IO stream (file, socket, etc)
  or String.
* Parse and encode *multiple* JSON objects to and from streams or strings
  continuously.
* JSON gem compatibility API - allows wankel to be used as a drop-in
  replacement for the JSON gem

Dependencies
------------

Wankel only dependency is [YAJL](http://lloyd.github.io/yajl/) version 2.

You can install it with homebrew via:

``` bash
brew install yajl
```

Installation
------------

Wankel is installed via Rubygems:

```
gem install wankel
```

Examples
--------

#### Simple Usage

```ruby
Wankel.parse('{"key": "value"}')
# => {"key" => "value"}

Wankel.encode({"key" => "value"})
# => '{"key":"value"}'
```

#### Sax Parser Example

```ruby
class SimpleParser < Wankel::SaxParser
  def on_array_start
    puts "Array start"
  end
  def on_string(string)
    puts string
  end
end

parser = SimpleParser.new
parser.parse('["string1", null, "string2"]')
# => "Array start"
# => "string1"
# => "string2"
```

#### Sax Encoder Example

```ruby
output = StringIO.new
encoder = Wankel::SaxEncoder.new(output)
encoder.map_open
encoder.string("key")
encoder.number(123)
encoder.map_close
encoder.complete
output.rewind
output.read
# => '{"key":123}'
```

Parsing Options
---------------

* `:symbolize_keys` (Default: `false`)
  
  make Hash keys into Ruby symbols

  ``` ruby
  Wankel.parse('{"key": "value"}', :symbolize_keys => true)
  # => {:key => "value"}
  ```

* `:allow_comments` (Default: `false`)

  Ignore javascript style comments in JSON input.

  ``` ruby
  Wankel.parse('{"key": /*comment*/ "value"}', :allow_comments => false)
  # => Wankel::ParseError

  Wankel.parse('{"key": /*comment*/ "value"}', :allow_comments => true)
  # => {"key" => "value"}
  ```

* `:validate_utf8` (Default: `false`)

  Verify that all strings in JSON input are valid UTF8 emit a parse error if
  this is not so. This option makes parsing slightly more expensive.

  ``` ruby
  Wankel.parse("[\"\x81\x83\"]", :validate_utf8 => false)
  # => ["\x81\x83"]

  Wankel.parse("[\"\x81\x83\"]", :validate_utf8 => true)
  # => Wankel::ParseError
  ```

* `:allow_trailing_garbage` (Default: `false`)

  Ensure the entire input text was consumed and raise an error otherwise

  ``` ruby
  Wankel.parse('{"key": "value"}gar', :allow_trailing_garbage => false)
  # => Wankel::ParseError
  
  Wankel.parse('{"key": "value"}gar', :allow_trailing_garbage => true)
  # => {"key" => "value"}
  ```

* `:multiple_values` (Default: `false`)

  Allow multiple values to be parsed by a single parser.
  The entire text must be valid JSON, and values can be seperated
  by any kind of whitespace

  ``` ruby
  Wankel.parse('{"abc": 123}{"def": 456}')
  # => Wankel::ParseError
  
  Wankel.parse('{"abc": 123}{"def": 456}', :multiple_values => true)
  # => [{"abc"=>123}, {"def"=>456}]

  Wankel.parse('{"abc": 123}{"def": 456}', :multiple_values => true) do |obj|
      puts obj
  end
  # => {"abc"=>123}
  # => {"def"=>456}
  ```

* `:allow_partial_values` (Default: `false`)

  TODO

* `:read_buffer_size` (Default: `8092`)
  
  The read buffer size in bytes when reading from an `IO` Object.


Encoding Options
----------------

* `:beautify` (Default: `false`)
  
  Generate indented (beautiful) output.

  ``` ruby
  Wankel.encode({"key" => "value"}, :beautify => true)
  # => "{\n    \"key\": \"value\"\n}\n"
  ```

* `:indent_string` (Default: `"    "`, four spaces)
  
  The indent string which is used when `:beautify` is enabled

  ``` ruby
  Wankel.encode({"key" => "value"}, :beautify => true, :indent_string => "\t")
  # => "{\n\t\"key\": \"value\"\n}\n"
  ```

* `:validate_utf8` (Default: `false`)
  
  Validate that the strings are valid UTF8.

  ``` ruby
  Wankel.encode(["#{"\201\203"}"], :validate_utf8 => false)
  # => "[\"#{"\201\203"}\"]"
  
  Wankel.encode(["#{"\201\203"}"], :validate_utf8 => true)
  # => Wankel::EncodeError
  ```

* `:escape_solidus` (Default: `false`)
  
  The forward solidus (slash or '/' in human) is not required to be escaped in
  json text. By default, YAJL will not escape it in the iterest of saving bytes.
  Setting this flag will cause YAJL to always escape '/' in generated JSON
  strings.
  
  ``` ruby
  Wankel.encode("/", :escape_solidus => false)
  # => '"/"'
  
  Wankel.encode("/", :escape_solidus => true)
  # => '"\/"'
  ```

* `:write_buffer_size`

  The write buffer size in bytes before writing to the `IO` object or `String`.

Performance Comparisons
-----------------------

TODO:

#### General Encoder

The test description (ie. Small: 100,000 parses of a 255 byte JSON):

| Gem           | Parse Time  | Rate            | Ratio |
|:------------- |:----------- |:--------------- |:----- |
| Wankel        | N seconds   | N.NK parses/sec | N.N   |
| OJ            | N seconds   | N.NK parses/sec | N.N   |
| Yajl          | N seconds   | N.NK parses/sec | N.N   |
| JSON::Ext     | N seconds   | N.NK parses/sec | N.N   |

#### General Decoder

#### Streaming Encoder

#### Streaming Decoder
