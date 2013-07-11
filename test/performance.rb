# To make testing/debugging easier, test within this source tree versus an
# installed gem
dir = File.dirname(__FILE__)
root = File.expand_path(File.join(dir, '..'))
lib = File.expand_path(File.join(root, 'lib'))

$LOAD_PATH << lib

require 'oj'
require 'json'
require 'yajl'
require 'wankel'


class Perf

  def initialize()
    @items = []
  end

  def add(title, op, &blk)
    @items << Item.new(title, op, &blk)
  end

  def before(title, &blk)
    @items.each do |i|
      if title == i.title
        i.set_before(&blk)
        break
      end
    end
  end
  
  def run(iter)
    base = Item.new(nil, nil) { }
    base.run(iter, 0.0)
    @items.each do |i|
      i.run(iter, base.duration)
      if i.error.nil?
        puts "#{i.title}.#{i.op} #{iter} times in %0.3f seconds or %0.3f #{i.op}/sec." % [i.duration, iter / i.duration]
      else
        puts "***** #{i.title}.#{i.op} failed! #{i.error}"
      end
    end
    summary()
  end

  def summary()
    fastest = nil
    slowest = nil
    width = 6
    @items.each do |i|
      next if i.duration.nil?
      width = i.title.size if width < i.title.size
    end
    iva = @items.clone
    iva.delete_if { |i| i.duration.nil? }
    iva = iva.sort_by { |i| i.duration }
    puts
    puts "Summary:"
    puts "%*s  time (secs)  rate (ops/sec)" % [width, 'System']
    puts "#{'-' * width}  -----------  --------------"
    iva.each do |i|
      if i.duration.nil?
      else
        puts "%*s %11.3f  %14.3f" % [width, i.title, i.duration, i.rate ]
      end
    end
    puts
    puts "Comparison Matrix\n(performance factor, 2.0 means row is twice as fast as column)"
    puts ([' ' * width] + iva.map { |i| "%*s" % [width, i.title] }).join('  ')
    puts (['-' * width] + iva.map { |i| '-' * width }).join('  ')
    iva.each do |i|
      line = ["%*s" % [width, i.title]]
      iva.each do |o|
        line << "%*.2f" % [width, o.duration / i.duration]
      end
      puts line.join('  ')
    end
    puts
  end

  class Item
    attr_accessor :title
    attr_accessor :op
    attr_accessor :blk
    attr_accessor :duration
    attr_accessor :rate
    attr_accessor :error

    def initialize(title, op, &blk)
      @title = title
      @blk = blk
      @op = op
      @duration = nil
      @rate = nil
      @error = nil
      @before = nil
    end

    def set_before(&blk)
      @before = blk
    end

    def run(iter, base)
      begin
        GC.start
        @before.call unless @before.nil?
        start = Time.now
        iter.times { @blk.call }
        @duration = Time.now - start - base
        @duration = 0.0 if @duration < 0.0
        @rate = iter / @duration
      rescue Exception => e
        @error = "#{e.class}: #{e.message}"
      end
    end

  end # Item
end # Perf


@json = '{"a":"Alpha","b":true,"c":12345,"d":[true,[false,{"12345":12345,"nil":null},3.967,{"x":"something","y":false,"z":true},null]],"e":{"one":1,"two":2},"f":null,"g":12345678901234567890123456789,"h":{"a":{"b":{"c":{"d":{"e":{"f":{"g":null}}}}}}},"i":[[[[[[[null]]]]]]]}'
@json = '{}'
puts '-' * 80
puts "Parse Performance"
perf = Perf.new()
perf.add('JSON::Ext', 'parse') { JSON::Ext::Parser.new(@json).parse }
perf.add('Yajl', 'parse') { Yajl::Parser.parse(@json) }
perf.add('Oj::Doc', 'parse') { Oj.load(@json) }
parser = Wankel::Parser.new
perf.add('Wankel', 'parse') { parser.parse(@json) }
perf.run(100000)