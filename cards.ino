// Edit this file to manage the card database.
// UIDs are placeholders — replace with real values printed in Serial Monitor.
// Format: { "UID", "Name", score, ABILITY }, // Faction | Row [| notes]
const CardDef cardPool[] = {

  // ── NEUTRAL ─────────────────────────────────────────────────────────────
  { "00:00:00:00:00:00:01", "Geralt of Rivia",               15, ABILITY_HERO             }, // Neutral | Melee
  { "00:00:00:00:00:00:02", "Ciri",                          15, ABILITY_HERO             }, // Neutral | Melee
  { "00:00:00:00:00:00:03", "Triss Merigold",                 7, ABILITY_HERO             }, // Neutral | Melee
  { "00:00:00:00:00:00:04", "Yennefer of Vengerberg",         7, ABILITY_HERO             }, // Neutral | Ranged | Medic
  { "00:00:00:00:00:00:05", "Mysterious Elf",                 0, ABILITY_HERO             }, // Neutral | Melee  | Spy
  { "00:00:00:00:00:00:06", "Dandelion",                      2, ABILITY_COMMANDERS_HORN  }, // Neutral | Melee
  { "00:00:00:00:00:00:07", "Emiel Regis",                    5, ABILITY_NONE             }, // Neutral | Melee
  { "00:00:00:00:00:00:08", "Gaunter O'Dimm",                 2, ABILITY_MUSTER           }, // Neutral | Siege
  { "00:00:00:00:00:00:09", "Gaunter O'Dimm: Darkness",       4, ABILITY_MUSTER           }, // Neutral | Ranged
  { "00:00:00:00:00:00:0A", "Olgierd von Everec",             6, ABILITY_MORALE_BOOST     }, // Neutral | Melee/Ranged | Agile
  { "00:00:00:00:00:00:0B", "Roach",                          3, ABILITY_NONE             }, // Neutral | Melee
  { "00:00:00:00:00:00:0C", "Vesemir",                        6, ABILITY_NONE             }, // Neutral | Melee
  { "00:00:00:00:00:00:0D", "Villentretenmerth",              7, ABILITY_DESTROY_STRONGEST}, // Neutral | Melee  | Scorch-Melee
  { "00:00:00:00:00:00:0E", "Zoltan Chivay",                  5, ABILITY_NONE             }, // Neutral | Melee
  { "00:00:00:00:00:00:0F", "Bovine Defense Force",           8, ABILITY_NONE             }, // Neutral | Melee  | Summoned by Cow removal
  { "00:00:00:00:00:00:10", "Cow",                            0, ABILITY_NONE             }, // Neutral | Ranged | Summons Bovine Defense Force when removed
  { "00:00:00:00:00:00:11", "Scorch",                         0, ABILITY_DESTROY_STRONGEST}, // Neutral | Special
  { "00:00:00:00:00:00:12", "Commander's Horn",               0, ABILITY_COMMANDERS_HORN  }, // Neutral | Special
  { "00:00:00:00:00:00:13", "Decoy",                          0, ABILITY_NONE             }, // Neutral | Special
  { "00:00:00:00:00:00:14", "Biting Frost",                   0, ABILITY_NONE             }, // Neutral | Special | Weather: sets all Melee to 1
  { "00:00:00:00:00:00:15", "Impenetrable Fog",               0, ABILITY_NONE             }, // Neutral | Special | Weather: sets all Ranged to 1
  { "00:00:00:00:00:00:16", "Torrential Rain",                0, ABILITY_NONE             }, // Neutral | Special | Weather: sets all Siege to 1
  { "00:00:00:00:00:00:17", "Skellige Storm",                 0, ABILITY_NONE             }, // Neutral | Special | Weather: sets all Ranged and Siege to 1
  { "00:00:00:00:00:00:18", "Clear Weather",                  0, ABILITY_NONE             }, // Neutral | Special | Clears all weather effects

  // ── NORTHERN REALMS ─────────────────────────────────────────────────────
  { "00:00:00:00:00:00:19", "Vernon Roche",                  10, ABILITY_HERO             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:1A", "John Natalis",                  10, ABILITY_HERO             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:1B", "Philippa Eilhart",              10, ABILITY_HERO             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:1C", "Esterad Thyssen",               10, ABILITY_HERO             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:1D", "Ballista",                       6, ABILITY_NONE             }, // Northern Realms | Siege
  { "00:00:00:00:00:00:1E", "Blue Stripes Commando",          4, ABILITY_TIGHT_BOND       }, // Northern Realms | Melee
  { "00:00:00:00:00:00:1F", "Catapult",                       8, ABILITY_TIGHT_BOND       }, // Northern Realms | Siege
  { "00:00:00:00:00:00:20", "Crinfrid Reavers Dragon Hunter", 5, ABILITY_TIGHT_BOND       }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:21", "Dethmold",                       6, ABILITY_NONE             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:22", "Dun Banner Medic",               5, ABILITY_MEDIC            }, // Northern Realms | Siege
  { "00:00:00:00:00:00:23", "Kaedweni Siege Expert",          1, ABILITY_MORALE_BOOST     }, // Northern Realms | Siege
  { "00:00:00:00:00:00:24", "Keira Metz",                     5, ABILITY_NONE             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:25", "Poor Fucking Infantry",          1, ABILITY_TIGHT_BOND       }, // Northern Realms | Melee
  { "00:00:00:00:00:00:26", "Prince Stennis",                 5, ABILITY_SPY              }, // Northern Realms | Melee
  { "00:00:00:00:00:00:27", "Redanian Foot Soldier",          1, ABILITY_NONE             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:28", "Sabrina Glevissig",              4, ABILITY_NONE             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:29", "Sheldon Skaggs",                 4, ABILITY_NONE             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:2A", "Siege Tower",                    6, ABILITY_NONE             }, // Northern Realms | Siege
  { "00:00:00:00:00:00:2B", "Siegfried of Denesle",           5, ABILITY_NONE             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:2C", "Sigismund Dijkstra",             4, ABILITY_SPY              }, // Northern Realms | Melee
  { "00:00:00:00:00:00:2D", "Sile de Tansarville",            5, ABILITY_NONE             }, // Northern Realms | Ranged
  { "00:00:00:00:00:00:2E", "Thaler",                         1, ABILITY_SPY              }, // Northern Realms | Siege
  { "00:00:00:00:00:00:2F", "Trebuchet",                      6, ABILITY_NONE             }, // Northern Realms | Siege
  { "00:00:00:00:00:00:30", "Ves",                            5, ABILITY_NONE             }, // Northern Realms | Melee
  { "00:00:00:00:00:00:31", "Yarpen Zigrin",                  2, ABILITY_NONE             }, // Northern Realms | Melee

  // ── NILFGAARDIAN EMPIRE ──────────────────────────────────────────────────
  { "00:00:00:00:00:00:32", "Letho of Gulet",                10, ABILITY_HERO             }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:33", "Menno Coehoorn",                10, ABILITY_HERO             }, // Nilfgaard | Melee  | Medic
  { "00:00:00:00:00:00:34", "Morvran Voorhis",               10, ABILITY_HERO             }, // Nilfgaard | Siege
  { "00:00:00:00:00:00:35", "Tibor Eggebracht",              10, ABILITY_HERO             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:36", "Albrich",                        2, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:37", "Assire var Anahid",              6, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:38", "Black Infantry Archer",         10, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:39", "Cahir Mawr Dyffryn aep Ceallach",6, ABILITY_NONE            }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:3A", "Cynthia",                        4, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:3B", "Etolian Auxiliary Archers",      1, ABILITY_MEDIC            }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:3C", "Fringilla Vigo",                 6, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:3D", "Heavy Zerrikanian Fire Scorpion",10, ABILITY_NONE            }, // Nilfgaard | Siege
  { "00:00:00:00:00:00:3E", "Impera Brigade Guard",           3, ABILITY_TIGHT_BOND       }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:3F", "Morteisen",                      3, ABILITY_NONE             }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:40", "Nausicaa Cavalry Rider",         2, ABILITY_TIGHT_BOND       }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:41", "Puttkammer",                     3, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:42", "Rainfarn",                       4, ABILITY_NONE             }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:43", "Renuald aep Matsen",             5, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:44", "Rotten Mangonel",                3, ABILITY_NONE             }, // Nilfgaard | Siege
  { "00:00:00:00:00:00:45", "Shilard Fitz-Oesterlen",         7, ABILITY_SPY              }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:46", "Siege Engineer",                 6, ABILITY_NONE             }, // Nilfgaard | Siege
  { "00:00:00:00:00:00:47", "Siege Technician",               0, ABILITY_MEDIC            }, // Nilfgaard | Siege
  { "00:00:00:00:00:00:48", "Stefan Skellen",                 9, ABILITY_SPY              }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:49", "Sweers",                         2, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:4A", "Vanhemar",                       4, ABILITY_NONE             }, // Nilfgaard | Ranged
  { "00:00:00:00:00:00:4B", "Vattier de Rideaux",             4, ABILITY_SPY              }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:4C", "Vreemde",                        2, ABILITY_NONE             }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:4D", "Young Emissary",                 5, ABILITY_TIGHT_BOND       }, // Nilfgaard | Melee
  { "00:00:00:00:00:00:4E", "Zerrikanian Fire Scorpion",      5, ABILITY_NONE             }, // Nilfgaard | Siege

  // ── SCOIA'TAEL ───────────────────────────────────────────────────────────
  { "00:00:00:00:00:00:4F", "Eithne",                        10, ABILITY_HERO             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:50", "Iorveth",                       10, ABILITY_HERO             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:51", "Isengrim Faoiltiarna",          10, ABILITY_HERO             }, // Scoia'tael | Melee  | Morale Boost
  { "00:00:00:00:00:00:52", "Saesenthessis",                 10, ABILITY_HERO             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:53", "Barclay Els",                    6, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged
  { "00:00:00:00:00:00:54", "Ciaran aep Easnillien",          3, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged
  { "00:00:00:00:00:00:55", "Dennis Cranmer",                 6, ABILITY_NONE             }, // Scoia'tael | Melee
  { "00:00:00:00:00:00:56", "Dol Blathanna Archer",           4, ABILITY_NONE             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:57", "Dol Blathanna Scout",            6, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged
  { "00:00:00:00:00:00:58", "Dwarven Skirmisher",             3, ABILITY_MUSTER           }, // Scoia'tael | Melee
  { "00:00:00:00:00:00:59", "Elven Skirmisher",               2, ABILITY_MUSTER           }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:5A", "Filavandrel aen Fidhail",        6, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged
  { "00:00:00:00:00:00:5B", "Havekar Healer",                 0, ABILITY_MEDIC            }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:5C", "Havekar Smuggler",               5, ABILITY_MUSTER           }, // Scoia'tael | Melee
  { "00:00:00:00:00:00:5D", "Ida Emean aep Sivney",           6, ABILITY_NONE             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:5E", "Mahakaman Defender",             5, ABILITY_NONE             }, // Scoia'tael | Melee
  { "00:00:00:00:00:00:5F", "Milva",                         10, ABILITY_MORALE_BOOST     }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:60", "Riordain",                       1, ABILITY_NONE             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:61", "Schirru",                        8, ABILITY_DESTROY_STRONGEST}, // Scoia'tael | Siege  | Scorch-Siege
  { "00:00:00:00:00:00:62", "Toruviel",                       2, ABILITY_NONE             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:63", "Vrihedd Brigade Recruit",        4, ABILITY_NONE             }, // Scoia'tael | Ranged
  { "00:00:00:00:00:00:64", "Vrihedd Brigade Veteran",        5, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged
  { "00:00:00:00:00:00:65", "Yaevinn",                        6, ABILITY_AGILE            }, // Scoia'tael | Melee/Ranged

  // ── MONSTERS ─────────────────────────────────────────────────────────────
  { "00:00:00:00:00:00:66", "Imlerith",                      10, ABILITY_HERO             }, // Monsters | Melee
  { "00:00:00:00:00:00:67", "Draug",                         10, ABILITY_HERO             }, // Monsters | Melee
  { "00:00:00:00:00:00:68", "Leshen",                        10, ABILITY_HERO             }, // Monsters | Ranged
  { "00:00:00:00:00:00:69", "Kayran",                         8, ABILITY_HERO             }, // Monsters | Melee/Ranged | Morale Boost, Agile
  { "00:00:00:00:00:00:6A", "Arachas",                        4, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:6B", "Arachas Behemoth",               6, ABILITY_MUSTER           }, // Monsters | Siege
  { "00:00:00:00:00:00:6C", "Botchling",                      4, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:6D", "Celaeno Harpy",                  2, ABILITY_AGILE            }, // Monsters | Melee/Ranged
  { "00:00:00:00:00:00:6E", "Cockatrice",                     2, ABILITY_NONE             }, // Monsters | Ranged
  { "00:00:00:00:00:00:6F", "Crone: Brewess",                 6, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:70", "Crone: Weavess",                 6, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:71", "Crone: Whispess",                6, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:72", "Earth Elemental",                6, ABILITY_NONE             }, // Monsters | Siege
  { "00:00:00:00:00:00:73", "Endrega",                        2, ABILITY_NONE             }, // Monsters | Ranged
  { "00:00:00:00:00:00:74", "Fiend",                          6, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:75", "Fire Elemental",                 6, ABILITY_NONE             }, // Monsters | Siege
  { "00:00:00:00:00:00:76", "Foglet",                         2, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:77", "Forktail",                       5, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:78", "Frightener",                     5, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:79", "Gargoyle",                       2, ABILITY_NONE             }, // Monsters | Ranged
  { "00:00:00:00:00:00:7A", "Ghoul",                          1, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:7B", "Grave Hag",                      5, ABILITY_NONE             }, // Monsters | Ranged
  { "00:00:00:00:00:00:7C", "Griffin",                        5, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:7D", "Harpy",                          2, ABILITY_AGILE            }, // Monsters | Melee/Ranged
  { "00:00:00:00:00:00:7E", "Ice Giant",                      5, ABILITY_NONE             }, // Monsters | Siege
  { "00:00:00:00:00:00:7F", "Nekker",                         2, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:80", "Plague Maiden",                  5, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:81", "Toad",                           7, ABILITY_DESTROY_STRONGEST}, // Monsters | Ranged | Scorch-Ranged
  { "00:00:00:00:00:00:82", "Vampire: Bruxa",                 4, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:83", "Vampire: Ekimmara",              4, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:84", "Vampire: Fleder",                4, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:85", "Vampire: Garkain",               4, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:86", "Vampire: Katakan",               5, ABILITY_MUSTER           }, // Monsters | Melee
  { "00:00:00:00:00:00:87", "Werewolf",                       5, ABILITY_NONE             }, // Monsters | Melee
  { "00:00:00:00:00:00:88", "Wyvern",                         2, ABILITY_NONE             }, // Monsters | Ranged

  // ── SKELLIGE ─────────────────────────────────────────────────────────────
  { "00:00:00:00:00:00:89", "Cerys",                         10, ABILITY_HERO             }, // Skellige | Melee  | Summons Shield Maidens
  { "00:00:00:00:00:00:8A", "Ermion",                         8, ABILITY_HERO             }, // Skellige | Ranged | Mardroeme
  { "00:00:00:00:00:00:8B", "Hjalmar",                       10, ABILITY_HERO             }, // Skellige | Ranged
  { "00:00:00:00:00:00:8C", "Hemdall",                       11, ABILITY_HERO             }, // Skellige | Melee  | Spawned by Kambi
  { "00:00:00:00:00:00:8D", "Berserker",                      4, ABILITY_NONE             }, // Skellige | Melee  | Transforms with Mardroeme
  { "00:00:00:00:00:00:8E", "Birna Bran",                     2, ABILITY_MEDIC            }, // Skellige | Melee
  { "00:00:00:00:00:00:8F", "Blueboy Lugos",                  6, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:90", "Clan Brokvar Archer",            6, ABILITY_NONE             }, // Skellige | Ranged
  { "00:00:00:00:00:00:91", "Clan Dimun Pirate",              6, ABILITY_DESTROY_STRONGEST}, // Skellige | Ranged | Scorch
  { "00:00:00:00:00:00:92", "Clan Drummond Shield Maiden",    4, ABILITY_TIGHT_BOND       }, // Skellige | Melee
  { "00:00:00:00:00:00:93", "Clan Heymaey Skald",             4, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:94", "Clan Tordarroch Armorsmith",     4, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:95", "Clan an Craite Warrior",         6, ABILITY_TIGHT_BOND       }, // Skellige | Melee
  { "00:00:00:00:00:00:96", "Donar an Hindar",                4, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:97", "Draig Bon-Dhu",                  2, ABILITY_COMMANDERS_HORN  }, // Skellige | Siege
  { "00:00:00:00:00:00:98", "Holger Blackhand",               4, ABILITY_NONE             }, // Skellige | Siege
  { "00:00:00:00:00:00:99", "Kambi",                          0, ABILITY_NONE             }, // Skellige | Melee  | Transforms into Hemdall when removed
  { "00:00:00:00:00:00:9A", "Light Longship",                 4, ABILITY_MUSTER           }, // Skellige | Ranged
  { "00:00:00:00:00:00:9B", "Madman Lugos",                   6, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:9C", "Olaf",                          12, ABILITY_MORALE_BOOST     }, // Skellige | Melee/Ranged | Agile
  { "00:00:00:00:00:00:9D", "Svanrige",                       4, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:9E", "Udalryk",                        4, ABILITY_NONE             }, // Skellige | Melee
  { "00:00:00:00:00:00:9F", "War Longship",                   6, ABILITY_TIGHT_BOND       }, // Skellige | Siege
  { "00:00:00:00:00:00:A0", "Young Berserker",                2, ABILITY_NONE             }, // Skellige | Ranged | Transforms with Mardroeme
  { "00:00:00:00:00:00:A1", "Mardroeme",                      0, ABILITY_NONE             }, // Skellige | Special | Transforms Berserker cards on same row
};

const uint8_t CARD_POOL_SIZE = sizeof(cardPool) / sizeof(cardPool[0]);
