class Wankel
  class StreamEncoder
  
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
end