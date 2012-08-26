class Wankel::SaxParser

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

  def on_map_key(key)
    puts 'key'
  end

  def on_start_map
    puts 'start map'
  end

  def on_end_map
    puts 'end map'
  end

  def on_start_array
    puts "start array"
  end

  def on_end_array
    puts "end array"
  end

end
