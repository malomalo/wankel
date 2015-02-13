require 'wankel/wankel'

class Wankel

  # [read_buffer_size]       The size of the read buffer in bytes. Deafults to
  #                          +8092+.
  # [symbolize_keys]         Set the keys of hashes to symbols instead of
  #                          strings. Defaults to +false+.
  # [allow_comments]         Ignore JavaScript style comments present in JSON
  #                          input. Defaults to +false+.
  # [validate_utf8]          Verify that all strings in the JSON input is valid
  #                          UTF-8. Defaults to +false+.
  # [allow_trailing_garbage] Don't raise an error there is content after the end
  #                          of the JSON document. Defaults to +false+.
  # [multiple_values]        Allow multiple JSON documents to be parsed. Every
  #                          document must be valid and seperated by whitespace
  #                          of any kind. The parser will continue to operate
  #                          on input rather going into a completed state.
  #                          Useful for parsing multiple values from an input
  #                          stream. Defaults to +false+.
  # [allow_partial_values]   The parser will check the top level value was
  #                          completely consumed by default, setting this allows
  #                          partial JSON values. Defaults to +false+.
  # [mode]                   The mode used for generate JSON. Setting mode to 
  #                          +:strict+ mode only allows the 7 basic JSON types.
  #                          Setting the mode to +:nil+ replaces an object that is
  #                          not a basic JSON type with null. If any other value
  #                          is passed it will call the object method with the
  #                          same name to serialize it to JSON. Defaults to
  #                          +:strict+
  # [write_buffer_size]      The size of the write buffer in bytes. Defaults to
  #                          +8092+.
  # [beautify]               Generate indented (beautiful) output. Defaults to
  #                          +false+.
  # [indent_string]          The indent string used when beautify is set.
  #                          Defaults to +'    '+ (4 spaces).
  # [validate_utf8]          Verify that all strings in JSON input are valid
  #                          UTF-8. Defaults to +false+.
  # [escape_solidus]         Alway escape the forward solidus ('/'), even though
  #                          it is not required in JSON strings. Defaults to +false+.
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
    :escape_solidus => false
  }
end

require 'wankel/stream_encoder'