# German strings – table only

Covers **v0.7.6** (`installer-v0.7.6`, commit 60704f8). Translation table only: no code, manifest or build change.

| file | what is in it |
|---|---|
| `xml-strings.csv` | 106 texts from the plugin XML (`manifests/*.xml`), UTF-8 |
| `runtime-strings.csv` | 197 texts the plugins and workers set at run time (`src/…`) |

## Columns

| column | meaning |
|---|---|
| app | app name |
| source | file in `source/native/` where the English text lives |
| kind | runtime table only: `text`, `gauge name`, `tile caption` (max 9 characters) or `narrow caption` |
| english | original text, as in the source |
| german | translation |
| written_as | runtime table only: the text as it has to be written in the C/C++ source (see below) |
| display_test | `seen in car` or `awaiting display test` |

## Encoding of runtime texts

The HMI shows texts set by the plugins as ISO-8859-1 (#170): lower-case umlauts cut the string, capitals
Ä/Ö/Ü (0xC4/0xD6/0xDC) display correctly. So `written_as` uses ae/oe/ue/ss for normal texts, and gauge names
and captions are all capitals – where they contain Ä/Ö/Ü, the C source needs the single byte (`\304`, `\326`,
`\334`), not UTF-8. XML texts are UTF-8 and keep every umlaut.

Lengths: tile captions are at most 9 characters (checked in the car). Two normal texts (`Gauge %u: %s`, `Bars`)
and some gauge names are longer than the English original.

## Display test

Seen on screen in our car (NBT Evo): BoostDeck, EngineCodes, ShiftDeck, MhdMaps, WarmDeck.
Awaiting display test: HybridDeck, MgFlasher, BEAMer, Bootmod3Suite, XDriveControl.

Not covered: LaunchDeck, RoadJet. Kept in English on purpose: AFR, Max Cooling, Burbles, Active Sound,
colour names (BMW Orange, Cyan, Pink, …), app names.
