<a href="vikingjs.org">![Wankel](/logo.jpg)</a>

Wankel is a Ruby gem that provides C bindings to the [YAJL 2](http://lloyd.github.io/yajl/)
C Library.

Wankel provides a gerneral parsing and encoding, but also a SAX style parser for
stream parsing and generation.

Features
--------

* JSON parsing and encoding directly to and from an IO stream (file, socket, etc)
  or String.
* Parse and encode *multiple* JSON objects to and from streams or strings
  continuously.
* JSON gem compatibility API - allows wankel to be used as a drop-in
  replacement for the JSON gem

Dependencies
------------

Wankel only dependency is (YAJL)[http://lloyd.github.io/yajl/] version 2.

You can install it with homebrew via:

``` bash
brew install yajl
```

Installation
------------

Wankel is installed via Rubygems:

```
gem install wankel
```

Examples
--------

TODO