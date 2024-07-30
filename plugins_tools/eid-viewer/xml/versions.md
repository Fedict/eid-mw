Versions of the XSD format
==========================

Sometimes, the format of the eID XML needs to be updated to be able to
represent new values and/or fields that may occur on the Belgian eID
card.

This file lists the changes that were made to the file format.

Version 4.0
-----------

Initial version of the file format.

Version 4.1
-----------

New attributes:

- Toplevel `&lt;eid&gt;` element adds the `version` attribute, to
  distinguish from older versions of the XML format.
- `&lt;identity&gt; adds the `memberoffamily` boolean attribute and the
  `dateandcountryofprotection` attribute

New elements:

- `&lt;workpermit&gt;` child element of the `&lt;card&gt;` element.

Version 4.2
-----------

New elements:

- `&lt;brexit&gt;` child element of the `&lt;card&gt;` element

Version 4.3
-----------

New allowed values:

- `ict\_i`, `mobile\_ict\_j`, `foreigner\_m`, and `foreigner\_n` for the
  `documenttype` attribute of the `&lt;card&gt;` element.

Version 4.4
-----------

New attributes:

- `graphpersoversion` for the `&lt;eid&gt;` toplevel element.

Version 4.5
-----------

New elements:

- `&lt;cardA&gt; child element of the `&lt;card&gt;` element
