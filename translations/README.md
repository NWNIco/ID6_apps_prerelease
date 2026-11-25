# German translation (v1.0)

`de-v1.0.csv` – German texts for the ID6 apps of v0.7.6 (UTF-8, comma-separated).

| column | meaning |
|---|---|
| app | app folder |
| file | file the text lives in |
| kind | `XML (UTF-8)` text in the plugin XML, `C/C++` runtime text set by the plugin, or gauge name / tile caption / narrow caption |
| english | original text |
| german | translation |
| bytes_on_unit | what is actually written on the unit |
| status | encoding note |

Runtime texts set by the plugins are shown as ISO-8859-1 by the HMI: lower-case umlauts cut the string, capitals
(0xC4/0xD6/0xDC) work (measured in a car). So runtime texts use ae/oe/ue/ss, gauge names and captions use capitals,
captions at most 9 characters. XML texts are UTF-8 and use every umlaut.
