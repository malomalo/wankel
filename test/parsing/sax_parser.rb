# encoding: UTF-8
require 'test_helper'

class TestParser < Wankel::SaxParser
  def on_map_start
  end

  def on_map_end
  end

  def on_map_key(key)
  end
  
  def on_null
  end

  def on_boolean(value)
  end

  def on_integer(i)
  end

  def on_double(d)
  end

  def on_string(s)
  end

  def on_array_start
  end

  def on_array_end
  end
  
end

class Wankel::SaxParserTest < ::Test::Unit::TestCase
  
  test "should not be able to parse #{File.basename(name)} as an IO" do
      parser = TestParser.new
      parser.expects(:on_map_start)
      parser.parse('{}')
  end
  
end

__END__

class Wankel::SaxParser

  def on_map_start
    puts 'start map'
  end

  def on_map_end
    puts 'end map'
  end

  def on_map_key(key)
    puts 'key'
  end
  
  def on_null
    puts "null"
  end

  def on_boolean(value)
    puts value
  end

  def on_integer(i)
    puts "integer"
  end

  def on_double(d)
    puts "double"
  end

  def on_string(s)
    puts "string"
  end

  def on_array_start
    puts "start array"
  end

  def on_array_end
    puts "end array"
  end

end