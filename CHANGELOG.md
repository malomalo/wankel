## Edge

New Features:

Major Changes:

Minor Changes:

Bugfixes:

## 0.6.2 (August 27th, 2015)

Bugfixes:

 - Fix downstream sprintf arguments through rb_raise

## 0.6.1 (February 19th, 2015)

Minor Changes:

 - Added abilitly to get the output stream via `Wankel::SaxEncoder#output`

## 0.6.0 (February 3rd, 2015)

New Features:

 - Added `:mode` option for the `Wankel::StreamEncoder`

## 0.5.0 (December 24th, 2014)

- Renaming of `Wankel::SaxEncoder` and `Wankel::SaxParser` to
  `Wankel::StreamEncoder` and `Wankel::StreamParser`.

## 0.4.0 (December 23rd, 2014)

New Features:

- Added `Wankel::SaxEncoder#value(VALUE)`
- Abilitly to change the output stream of a `Wankel::SaxEncoder` during generation
  via `Wankel::SaxEncoder#output=`

Major Changes:

- `Wankel::SaxEncoder#complete` changed to `Wankel::SaxEncoder#flush`

## 0.3.0 (August 18th, 2014)

Major Changes:

  - `Wankel::SaxEncoder#bool` changed to `Wankel::SaxEncoder#boolean`
