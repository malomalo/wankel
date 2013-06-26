require 'test_helper'
require 'tmpdir'
require 'zlib'

class Dummy2
  def to_json
    "{\"hawtness\":true}"
  end
end

class TheMindKiller
  def to_json
    nil
  end
end

class TheMindKillerDuce
  def to_s
    nil
  end
end

class Wankel::EncoderTest < ::Test::Unit::TestCase
  FILES = Dir[File.dirname(__FILE__)+'/../../benchmark/subjects/*.json']
  
  FILES.each do |file|
    if File.basename(file) != 'twitter_stream.json'
      test "should encode #{File.basename(file)} to an StringIO" do
         input = File.new(File.expand_path(file), 'r')
         io = StringIO.new
         encoder = Wankel::Encoder.new
         hash = Wankel.parse(input)
         encoder.encode(hash, io)
         io.rewind
         hash2 = Wankel.parse(io)
         io.close
         input.close
         assert_equal(hash2, hash)
       end
     end
   end
  
   FILES.each do |file|
     test "should encode #{File.basename(file)} to a Zlib::GzipWriter" do
      # we don't care about testing the stream subject as it has multiple JSON strings in it
      if File.basename(file) != 'twitter_stream.json'
         hash = File.open(File.expand_path(file), 'r') do |input|
           Wankel.parse(input)
         end
         hash2 = Dir.mktmpdir do |tmp_dir|
            output_filename = File.join(tmp_dir, 'output.json')
            Zlib::GzipWriter.open(output_filename) do |writer|
               Wankel.encode(hash, writer)
            end
            Zlib::GzipReader.open(output_filename) do |reader|
               Wankel.parse(reader.read)
            end
         end
         assert_equal(hash2, hash)
      end
     end
   end
  
   FILES.each do |file|
     test "should encode #{File.basename(file)} and return a String" do
       # we don't care about testing the stream subject as it has multiple JSON strings in it
       if File.basename(file) != 'twitter_stream.json'
         input = File.new(File.expand_path(file), 'r')
         encoder = Wankel::Encoder.new
         hash = Wankel.parse(input)
         output = encoder.encode(hash)
         hash2 = Wankel.parse(output)
         input.close
         assert_equal(hash2, hash)
       end
     end
   end
  
  test "should encode with :beautify turned on and a single space indent, to an IO" do
    output = "{\n \"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    io = StringIO.new
    encoder = Wankel::Encoder.new(:beautify => true, :indent_string => ' ')
    encoder.encode(obj, io)
    io.rewind
    assert_equal(output, io.read)
  end
  
  test "should encode with :beautify turned on and a single space indent, and return a String" do
    output = "{\n \"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    encoder = Wankel::Encoder.new(:beautify => true, :indent_string => ' ')
    result = encoder.encode(obj)
    assert_equal(output, result)
  end
  
  test "should encode with :beautify turned on and a tab character indent, to an IO" do
    output = "{\n\t\"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    io = StringIO.new
    encoder = Wankel::Encoder.new(:beautify => true, :indent_string => "\t")
    encoder.encode(obj, io)
    io.rewind
    assert_equal(output, io.read)
  end
  
  test "should encode with :beautify turned on and a tab character indent, and return a String" do
    output = "{\n\t\"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    encoder = Wankel::Encoder.new(:beautify => true, :indent_string => "\t")
    result = encoder.encode(obj)
    assert_equal(output, result)
  end
  
  test "should encode with it's class method with :beautify and a tab character indent options set, to an IO" do
    output = "{\n\t\"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    io = StringIO.new
    Wankel.encode(obj, io, :beautify => true, :indent_string => "\t")
    io.rewind
    assert_equal(output, io.read)
  end
  
  test "should encode with it's class method with :beautify and a tab character indent options set, and return a String" do
    output = "{\n\t\"foo\": 1234\n}\n"
    obj = {:foo => 1234}
    result = Wankel.encode(obj, :beautify => true, :indent_string => "\t")
    assert_equal(output, result)
  end
  
  test "should encode multiple objects into a single stream, to an IO" do
    io = StringIO.new
    obj = {:foo => 1234}
    encoder = Wankel::Encoder.new
    5.times do
      encoder.encode(obj, io)
    end
    io.rewind
    output = "{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}"
    assert_equal(output, io.read)
  end
  
  test "should encode multiple objects into a single stream, and return a String" do
    obj = {:foo => 1234}
    encoder = Wankel::Encoder.new
    json_output = ''
    5.times do
      json_output << encoder.encode(obj)
    end
    output = "{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}{\"foo\":1234}"
    assert_equal(output, json_output)
  end
  
  test "should encode all map keys as strings" do
    assert_equal("{\"1\":1}", Wankel.encode({1=>1}))
  end
  
  test "should check for and call #to_json if it exists on custom objects" do
    d = Dummy2.new
    assert_equal('{"foo":{"hawtness":true}}', Wankel.encode({:foo => d}))
  end
  
  test "should encode a hash where the key and value can be symbols" do
    assert_equal('{"foo":"bar"}', Wankel.encode({:foo => :bar}))
  end
  
  test "should not encode NaN" do
    assert_raises Wankel::EncodeError do
      Wankel.encode(0.0/0.0)
    end
  end
  
  test "should not encode Infinity or -Infinity" do
    assert_raises Wankel::EncodeError do
      Wankel.encode(1.0/0.0)
    end
    assert_raises Wankel::EncodeError do
      Wankel.encode(-1.0/0.0)
    end
  end
  
  test "should encode with unicode chars in the key" do
    hash = {"浅草" => "<- those are unicode"}
    assert_equal("{\"浅草\":\"<- those are unicode\"}", Wankel.encode(hash))
  end

  test "should return a string encoded in utf-8 if Encoding.default_internal is nil" do
    Encoding.default_internal = nil
    hash = {"浅草" => "<- those are unicode"}
    assert_equal(Encoding.find('utf-8'), Wankel.encode(hash).encoding)
  end

  test "should return a string encoded in utf-8 even if Encoding.default_internal *is* set" do
    Encoding.default_internal = Encoding.find('utf-8')
    hash = {"浅草" => "<- those are unicode"}
    assert_equal(Encoding.default_internal, Wankel.encode(hash).encoding)
    
    Encoding.default_internal = Encoding.find('us-ascii')
    hash = {"浅草" => "<- those are unicode"}
    assert_equal(Encoding.find('utf-8'), Wankel.encode(hash).encoding)
  end
  
  test "should default to *not* escaping / characters" do
    unsafe_encoder = Wankel::Encoder.new
    assert_equal("\"</script>\"", unsafe_encoder.encode("</script>"))
  end
  
  test "return value of #to_json must be a string" do
    assert_raises TypeError do
      Wankel.encode(TheMindKiller.new)
    end
  end
  
  test "return value of #to_s must be a string" do
    assert_raises TypeError do
      if TheMindKillerDuce.send(:method_defined?, :to_json)
        TheMindKillerDuce.send(:undef_method, :to_json)
      end
      Wankel.encode(TheMindKillerDuce.new)
    end
  end
end