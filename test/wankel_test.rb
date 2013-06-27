require 'test_helper'

class WankelTest < ::Test::Unit::TestCase

  test "dump should exist as a class-method" do
    assert(Wankel.respond_to?(:dump))
  end

  test "dump should be able to encode to a string" do
    assert_equal('{"a":1234}', Wankel.dump({:a => 1234}))
  end

  test "dump should be able to encode to an IO" do
    io = StringIO.new
    Wankel.dump({:a => 1234}, io)
    io.rewind
    assert_equal('{"a":1234}', io.read)
  end

  test "load should exist as a class-method" do
    assert(Wankel.respond_to?(:load))
  end

  test "load should be able to parse from a string" do
    assert_equal({"a" => 1234}, Wankel.load('{"a":1234}'))
  end

  test "load should be able to parse from an IO" do
    io = StringIO.new('{"a":1234}')
    assert_equal({"a" => 1234}, Wankel.load(io))
  end

  test "load should be able to parse from a string with a block supplied" do
    Wankel.load('{"a":1234}') do |h|
      assert_equal({"a" => 1234}, h)
    end
  end

  test "load should be able to parse from an IO with a block supplied" do
    io = StringIO.new('{"a":1234}')
    Wankel.load(io) do |h|
      assert_equal({"a" => 1234}, h)
    end
  end
  
  test "If I cause a segfault after GC runs I've failed" do
    io = StringIO.new
    assert_raises TypeError do
      Wankel.parse({:a => 1234}, io)
    end
  end
  
end