require 'wankel/wankel'

class Wankel
  DEFAULTS = {
    :read_buffer_size => 8092,
    :write_buffer_size => 8092,
    :symbolize_keys => false,
    :allow_comments => true,
    :validate_strings => false,
    :allow_trailing_garbage => false,
    :multiple_values => false,
    :allow_partial_values => false
  }
end