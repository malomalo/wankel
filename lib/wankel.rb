require 'wankel/wankel'

class Wankel
    DEFAULTS = {
      :read_buffer_size => 8092,
      :symbolize_keys => false,
      :allow_comments => false,
      :validate_utf8 => false,
      :allow_trailing_garbage => false,
      :multiple_values => false,
      :allow_partial_values => false,
      
      :mode => :strict,
      :write_buffer_size => 8092,
      :beautify => false,
      :indent_string => '    ',
      :validate_utf8 => false,
      :escape_solidus => false
    }
end

class Wankel::StreamEncoder
  
  def value(val)
    case val
    when NilClass
      null
    when TrueClass, FalseClass
      boolean(val)
    when Numeric
      number(val)
    when String
      string(val)
    when Array
      array_open
      val.each {|v| value(v) }
      array_close
    when Hash
      map_open
      val.each {|k, v| string(k.to_s); value(v) }
      map_close
    else
      case @options[:mode]
      when :strict
        raise Wankel::EncodeError, "Unkown JSON type #{val.class}"
      when :nil
        null
      else
        value(val.send(@options[:mode]))
      end
    end
  end
  
end