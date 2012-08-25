class Wankel::SaxParser

  def on_null
    puts "null from ruby"
  end

  def on_boolean(value)
    puts "#{value} from ruby"
  end

  def on_integer(i)
    puts "#{i} from ruby on int"
  end

end
