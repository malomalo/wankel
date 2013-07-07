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


class Wankel::SaxEncoder

  def number
  end
  
  def string
  end
  
  def null
  end
  
  def bool
  end
  
  def map_open
  end
  
  def map_close
  end
  
  def array_open
  end
  
  def array_close
  end
  
  def complete
  end

end
