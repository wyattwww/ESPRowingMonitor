#include <numeric>

const unsigned long test[]{
    5331447,
    315088,
    154906,
    110027,
    89455,
    77505,
    69448,
    63130,
    58266,
    54404,
    50983,
    48365,
    46328,
    44393,
    42879,
    41641,
    40333,
    39263,
    38386,
    37347,
    36486,
    35762,
    34888,
    34231,
    33705,
    33029,
    32497,
    32044,
    31440,
    31022,
    30713,
    30267,
    29999,
    29873,
    29673,
    29722,
    29897,
    29880,
    29997,
    30185,
    30177,
    30288,
    30471,
    30473,
    30581,
    30769,
    30775,
    30879,
    31080,
    31074,
    31185,
    31393,
    31373,
    31506,
    31696,
    31685,
    31821,
    32000,
    32015,
    32125,
    32322,
    32328,
    32436,
    32654,
    32641,
    32755,
    32979,
    32956,
    33085,
    33300,
    33277,
    33413,
    33626,
    33599,
    33740,
    33954,
    33929,
    34064,
    34288,
    34267,
    34393,
    34611,
    34610,
    34738,
    34951,
    34940,
    35068,
    35282,
    35265,
    35395,
    35618,
    35608,
    35704,
    35735,
    35208,
    34572,
    34009,
    33196,
    32471,
    31869,
    31130,
    30549,
    30107,
    29505,
    29039,
    28699,
    28203,
    27824,
    27551,
    27122,
    26798,
    26589,
    26223,
    25960,
    25782,
    25439,
    25178,
    24999,
    24678,
    24460,
    24338,
    24071,
    23895,
    23813,
    23598,
    23481,
    23455,
    23285,
    23207,
    23220,
    23110,
    23126,
    23258,
    23252,
    23332,
    23473,
    23466,
    23552,
    23702,
    23691,
    23772,
    23926,
    24011,
    24156,
    24151,
    24250,
    24391,
    24383,
    24488,
    24628,
    24623,
    24725,
    24869,
    24861,
    24966,
    25106,
    25102,
    25206,
    25349,
    25347,
    25451,
    25598,
    25597,
    25704,
    25849,
    25961,
    26110,
    26103,
    26218,
    26370,
    26356,
    26470,
    26631,
    26608,
    26723,
    26887,
    26866,
    26982,
    27144,
    27130,
    27225,
    27411,
    27400,
    27494,
    27662,
    27657,
    27768,
    27942,
    27926,
    28031,
    28204,
    28192,
    28301,
    28475,
    28460,
    28563,
    28738,
    28726,
    28844,
    29016,
    28986,
    29059,
    29068,
    28708,
    28388,
    28103,
    27603,
    27234,
    26909,
    26409,
    26066,
    25797,
    25375,
    25105,
    24898,
    24530,
    24292,
    24119,
    23794,
    23611,
    23486,
    23193,
    23012,
    22880,
    22596,
    22438,
    22338,
    22089,
    21964,
    21894,
    21681,
    21590,
    21546,
    21355,
    21296,
    21301,
    21175,
    21174,
    21247,
    21197,
    21281,
    21418,
    21398,
    21475,
    21612,
    21601,
    21678,
    21814,
    21800,
    21882,
    22027,
    22016,
    22090,
    22230,
    22232,
    22301,
    22441,
    22442,
    22521,
    22652,
    22658,
    22736,
    22870,
    22874,
    22955,
    23087,
    23094,
    23175,
    23308,
    23314,
    23400,
    23533,
    23531,
    23628,
    23764,
    23752,
    23852,
    23996,
    23978,
    24079,
    24231,
    24211,
    24304,
    24466,
    24448,
    24534,
    24696,
    24691,
    24770,
    24927,
    24926,
    25015,
    25162,
    25152,
    25258,
    25412,
    25396,
    25486,
    25652,
    25642,
    25741,
    25892,
    25885,
    25977,
    26140,
    26130,
    26222,
    26386,
    26371,
    26465,
    26631,
    26625,
    26721,
    26873,
    26850,
    26875,
    26844,
    26571,
    26354,
    26168,
    25830,
    25577,
    25363,
    25026,
    24803,
    24632,
    24335,
    24133,
    23985,
    23707,
    23522,
    23387,
    23124,
    22963,
    22845,
    22601,
    22442,
    22325,
    22089,
    21947,
    21853,
    21649,
    21538,
    21472,
    21296,
    21215,
    21185,
    21053,
    21016,
    21030,
    20943,
    20962,
    21044,
    21036,
    21116,
    21242,
    21227,
    21303,
    21439,
    21430,
    21507,
    21637,
    21625,
    21707,
    21852,
    21835,
    21908,
    22053,
    22047,
    22118,
    22259,
    22330,
    22470,
    22544,
    22680,
    22685,
    22759,
    22899,
    22903,
    22979,
    23116,
    23121,
    23199,
    23338,
    23339,
    23424,
    23561,
    23556,
    23651,
    23790,
    23774,
    23876,
    24021,
    24000,
    24099,
    24254,
    24234,
    24325,
    24487,
    24471,
    24554,
    24716,
    24711,
    24793,
    24945,
    24944,
    25039,
    25187,
    25172,
    25274,
    25432,
    25420,
    25515,
    25670,
    25659,
    25754,
    25914,
    25900,
    25993,
    26154,
    26137,
    26236,
    26406,
    26387,
    26469,
    26612,
    26491,
    26357,
    26222,
    25874,
    25617,
    25439,
    25073,
    24796,
    24612,
    24274,
    24044,
    23892,
    23579,
    23362,
    23236,
    22955,
    22768,
    22674,
    22423,
    22274,
    22205,
    21963,
    21824,
    21755,
    21546,
    21436,
    21414,
    21249,
    21179,
    21191,
    21069,
    21045,
    21108,
    21040,
    21081,
    21214,
    21200,
    21268,
    21398,
    21393,
    21470,
    21602,
    21590,
    21668,
    21808,
    21803,
    21876,
    22004,
    22087,
    22215,
    22208,
    22300,
    22431,
    22422,
    22512,
    22645,
    22632,
    22727,
    22864,
    22849,
    22938,
    23088,
    23069,
    23149,
    23303,
    23298,
    23376,
    23518,
    23515,
    23605,
    23743,
    23734,
    23829,
    23981,
    23957,
    24053,
    24212,
    24195,
    24279,
    24441,
    24424,
    24510,
    24672,
    24664,
    24742,
    24904,
    24898,
    24986,
    25136,
    25131,
    25234,
    25382,
    25370,
    25463,
    25628,
    25619,
    25711,
    25873,
    25862,
    25951,
    26115,
    26099,
    26192,
    26361,
    26351,
    26439,
    26585,
    26536,
    26481,
    26349,
    25957,
    25628,
    25388,
    25020,
    24750,
    24547,
    24218,
    23984,
    23826,
    23539,
    23332,
    23186,
    22897,
    22690,
    22542,
    22281,
    22109,
    22004,
    21788,
    21645,
    21559,
    21347,
    21214,
    21153,
    20981,
    20895,
    20874,
    20745,
    20700,
    20725,
    20649,
    20664,
    20748,
    20730,
    20795,
    20915,
    20911,
    20987,
    21116,
    21103,
    21180,
    21310,
    21301,
    21375,
    21509,
    21499,
    21584,
    21716,
    21697,
    21779,
    21924,
    21913,
    21984,
    22127,
    22123,
    22194,
    22333,
    22331,
    22412,
    22543,
    22536,
    22628,
    22762,
    22746,
    22838,
    22985,
    22968,
    23047,
    23201,
    23194,
    23271,
    23411,
    23414,
    23499,
    23636,
    23623,
    23723,
    23869,
    23850,
    23939,
    24099,
    24082,
    24164,
    24324,
    24315,
    24394,
    24553,
    24548,
    24631,
    24781,
    24782,
    24869,
    25017,
    25015,
    25112,
    25259,
    25247,
    25343,
    25507,
    25496,
    25586,
    25744,
    25731,
    25828,
    25988,
    25973,
    26069,
    26224,
    26214,
    26317,
    26474,
    26436,
    26475,
    26440,
    26091,
    25820,
    25587,
    25200,
    24938,
    24718,
    24331,
    24081,
    23899,
    23574,
    23375,
    23221,
    22919,
    22748,
    22633,
    22389,
    22274,
    22214,
    22009,
    21926,
    21869,
    21666,
    21575,
    21529,
    21346,
    21277,
    21260,
    21104,
    21059,
    21061,
    20932,
    20929,
    20966,
    20879,
    20911,
    21002,
    20965,
    21056,
    21183,
    21169,
    21241,
    21376,
    21367,
    21447,
    21576,
    21568,
    21644,
    21787,
    21778,
    21848,
    21986,
    21988,
    22058,
    22193,
    22195,
    22272,
    22404,
    22405,
    22483,
    22617,
    22618,
    22697,
    22834,
    22831,
    22916,
    23048,
    23049,
    23135,
    23267,
    23265,
    23356,
    23490,
    23480,
    23580,
    23718,
    23701,
    23802,
    23947,
    23929,
    24023,
    24179,
    24156,
    24247,
    24410,
    24390,
    24476,
    24639,
    24629,
    24711,
    24868,
    24865,
    24957,
    25107,
    25093,
    25195,
    25351,
    25338,
    25430,
    25588,
    25576,
    25669,
    25829,
    25817,
    25909,
    26068,
    26049,
    26152,
    26315,
    26295,
    26352,
    26409,
    26131,
    25852,
    25616,
    25204,
    24909,
    24691,
    24285,
    23992,
    23792,
    23435,
    23194,
    23035,
    22710,
    22494,
    22362,
    22079,
    21911,
    21831,
    21591,
    21464,
    21402,
    21182,
    21059,
    21013,
    20819,
    20727,
    20709,
    20541,
    20467,
    20470,
    20325,
    20281,
    20316,
    20212,
    20215,
    20307,
    20257,
    20322,
    20458,
    20442,
    20516,
    20640,
    20631,
    20708,
    20835,
    20822,
    20901,
    21029,
    21020,
    21096,
    21228,
    21220,
    21295,
    21430,
    21419,
    21494,
    21629,
    21618,
    21697,
    21832,
    21821,
    21900,
    22037,
    22027,
    22107,
    22243,
    22233,
    22313,
    22452,
    22441,
    22521,
    22662,
    22652,
    22734,
    22874,
    22864,
    22945,
    23090,
    23077,
    23161,
    23306,
    23296,
    23381,
    23527,
    23514,
    23601,
    23749,
    23736,
    23825,
    23972,
    23965,
    24051,
    24200,
    24189,
    24278,
    24428,
    24420,
    24506,
    24660,
    24649,
    24743,
    24892,
    24886,
    24975,
    25129,
    25123,
    25213,
    25372,
    25361,
    25453,
    25610,
    25594,
    25689,
    25849,
    25848,
    25942,
    26099,
    26085,
    26180,
    26352,
    26343,
    26424,
    26606,
    26589,
    26685,
    26863,
    26838,
    26956,
    27108,
    27113,
    27203,
    27380,
    27370,
    27467,
    27650,
    27623,
    27750,
    27899,
    27910,
    28003,
    28179,
    28181,
    28269,
    28456,
    28448,
    28544,
    28728,
    28723,
    28827,
    29001,
    28989,
    29100,
    29279,
    29268,
    29374,
    29550,
    29534,
    29650,
    29728,
    29471,
    29125,
    28697,
    28093,
    27622,
    27202,
    26636,
    26210,
    25859,
    25388,
    25063,
    24792,
    24406,
    24145,
    23931,
    23588,
    23374,
    23214,
    22937,
    22774,
    22655,
    22407,
    22271,
    22172,
    21951,
    21841,
    21761,
    21558,
    21466,
    21420,
    21258,
    21213,
    21208,
    21094,
    21103,
    21151,
    21094,
    21159,
    21269,
    21253,
    21341,
    21472,
    21457,
    21534,
    21668,
    21658,
    21733,
    21871,
    21865,
    21947,
    22082,
    22066,
    22152,
    22297,
    22279,
    22360,
    22511,
    22494,
    22571,
    22725,
    22707,
    22787,
    22940,
    22923,
    23002,
    23159,
    23144,
    23222,
    23379,
    23368,
    23446,
    23596,
    23594,
    23670,
    23816,
    23817,
    23903,
    24042,
    24037,
    24136,
    24275,
    24258,
    24368,
    24509,
    24492,
    24593,
    24748,
    24726,
    24817,
    24984,
    24969,
    25052,
    25211,
    25301,
    25453,
    25438,
    25538,
    25701,
    25689,
    25784,
    25940,
    25929,
    26021,
    26185,
    26172,
    26266,
    26437,
    26419,
    26503,
    26662,
    26575,
    26460,
    26306,
    25908,
    25604,
    25399,
    25030,
    24737,
    24520,
    24166,
    23916,
    23751,
    23435,
    23200,
    23046,
    22761,
    22565,
    22464,
    22230,
    22082,
    22010,
    21791,
    21654,
    21586,
    21386,
    21277,
    21254,
    21090,
    21013,
    20884,
    20844,
    20883,
    20788,
    20773,
    20849,
    20791,
    20834,
    20966,
    20957,
    21028,
    21157,
    21144,
    21223,
    21357,
    21350,
    21426,
    21559,
    21547,
    21623,
    21761,
    21758,
    21834,
    21961,
    21958,
    22045,
    22178,
    22162,
    22250,
    22393,
    22374,
    22455,
    22603,
    22591,
    22664,
    22812,
    22810,
    22883,
    23024,
    23107,
    23240,
    23237,
    23329,
    23468,
    23450,
    23547,
    23696,
    23675,
    23761,
    23922,
    23908,
    23989,
    24140,
    24143,
    24222,
    24365,
    24367,
    24459,
    24600,
    24591,
    24694,
    24840,
    24824,
    24925,
    25084,
    25063,
    25156,
    25320,
    25313,
    25399,
    25554,
    25545,
    25647,
    25806,
    25795,
    25938,
    25924,
    26018,
    26180,
    26165,
    26260,
    26423,
    26412,
    26507,
    26669,
    26659,
    26754,
    26920,
    26905,
    27005,
    27170,
    27154,
    27257,
    27422,
    27408,
    27511,
    27678,
    27664,
    27767,
    27933,
    27918,
    28023,
    28193,
    28176,
    28280,
    28455,
    28438,
    28545,
    28718,
    28706,
    28809,
    28987,
    28971,
    29078,
    29261,
    29243,
    29352,
    29534,
    29516,
    29628,
    29807,
    29792,
    29906,
    30084,
    30072,
    30185,
    30367,
    30354,
    30468,
    30656,
    30637,
    30753,
    30939,
    30927,
    31042,
    31229,
    31216,
    31331,
    31521,
    31508,
    31627,
    31819,
    31804,
    31924,
    32119,
    32103,
    32223,
    32419,
    32407,
    32526,
    32725,
    32708,
    32833,
    33032,
    33018,
    33144,
    33341,
    33329,
    33454,
    33659,
    33641,
    33772,
    33978,
    33959,
    34086,
    34296,
    34281,
    34409,
    34619,
    34603,
    34734,
    34945,
    34930,
    35056,
    35274,
    35260,
    35390,
    35609,
    35591,
    35724,
    35942,
    35927,
    36060,
    36281,
    36266,
    36404,
    36626,
    36611,
    36746,
    36969,
    36955,
    37093,
    37315,
    37303,
    37440,
    37668,
    37653,
    37794,
    38025,
    38007,
    38151,
    38385,
    38369,
    38515,
    38750,
    38732,
    38882,
    39118,
    39101,
    39252,
    39489,
    39476,
    39624,
    39868,
    39853,
    40003,
    40245,
    40232,
    40385,
    40630,
    40615,
    40769,
    41019,
    41006,
    41159,
    41411,
    41399,
    41553,
    41809,
    41796,
    41951,
    42207,
    42193,
    42350,
    42609,
    42596,
    42755,
    43019,
    43006,
    43169,
    43427,
    43417,
    43581,
    43848,
    43834,
    44001,
    44268,
    44250,
    44421,
    44689,
    44675,
    44845,
    45114,
    45101,
    45273,
    45551,
    45536,
    45712,
    45985,
    45979,
    46145,
    46430,
    46418,
    46592,
    46878,
    46863,
    47040,
    47329,
    47317,
    47492,
    47786,
    47766,
    47950,
    48241,
    48226,
    48413,
    48707,
    48684,
    48873,
    49168,
    49157,
    49345,
    49643,
    49629,
    49818,
    50123,
    50111,
    50304,
    50611,
    50602,
    50794,
    51102,
    51089,
    51283,
    51596,
    51585,
    51782,
    52097,
    52088,
    52287,
    52604,
    52593,
    52795,
    53113,
    53106,
    53308,
    53631,
    53619,
    53831,
    54152,
    54135,
    54348,
    54683,
    54669,
    54877,
    55215,
    55203,
    55417,
    55750,
    55743,
    55960,
    56300,
    56293,
    56513,
    56852,
    56846,
    57067,
    57412,
    57407,
    57630,
    57983,
    57975,
    58199,
    58555,
    58547,
    58774,
    59134,
    59128,
    59362,
    59724,
    59723,
    59957,
    60326,
    60327,
    60566,
    60938,
    60935,
    61177,
    61552,
    61795,
    62171,
    62174,
    62425,
    62806,
    62808,
    63053,
    63442,
    63445,
    63699,
    64089,
    64095,
    64344,
    64741,
    64745,
    64997,
    65400,
    65398,
    65659,
    66060,
    66067,
    66329,
    66744,
    66740,
    67010,
    67427,
    67435,
    67700,
    68120,
    68131,
    68404,
    68829,
    68839,
    69117,
    69544,
    69557,
    69834,
    70269,
    70283,
    70566,
    71008,
    71019,
    71303,
    71753,
    71768,
    72059,
    72507,
    72521,
    72819,
    73266,
    73288,
    73584,
    74046,
    74065,
    74371,
    74832,
    74859,
    75166,
    75639,
    75667,
    75976,
    76454,
    76483,
    76795,
    77283,
    77316,
    77634,
    78124,
    78159,
    78481,
    78977,
    79013,
    79341,
    79837,
    79877,
    80212,
    80710,
    80754,
    81091,
    81605,
    81649,
    81994,
    82509,
    82550,
    82899,
    83427,
    83473,
    83824,
    84354,
    84402,
    84766,
    85300,
    85354,
    85718,
    86266,
    86320,
    86692,
    87248,
    87302,
    87684,
    88237,
    88302,
    88684,
    89248,
    89314,
    89703,
    90276,
    90343,
    90738,
    91319,
    91398,
    91791,
    92379,
    92459,
    92873,
    93466,
    93553,
    93964,
    94572,
    94658,
    95073,
    95689,
    95784,
    96204,
    96834,
    96925,
    97362,
    97993,
    98097,
    98534,
    99174,
    99284,
    99725,
    100381,
    100487,
    100934,
    101594,
    101715,
    102172,
    102839,
    102958,
    103432,
    104107,
    104235,
    104711,
    105392,
    105528,
    106006,
    106701,
    106845,
    107335,
    108040,
    108188,
    108691,
    109409,
    109569,
    110075,
    110803,
    110971,
    111486,
    112224,
    112400,
    112924,
    113672,
    113850,
    114395,
    115155,
    115346,
    115895,
    116668,
    116874,
    117423,
    118216,
    118427,
    118992,
    119793,
    120018,
    120595,
    121404,
    121637,
    122226,
    123051,
    123297,
    123889,
    124728,
    124990,
    125603,
    126460,
    126728,
    127362,
    128231,
    128505,
    129150,
    130047,
    130354,
    131004,
    131919,
    132233,
    132905,
    133822,
    134164,
    134848,
    135778,
    136148,
    136851,
    137806,
    138177,
    138890,
    139866,
    140257,
    141000,
    141988,
    142407,
    143162,
    144186,
    144627,
    145405,
    146447,
    146898,
    147673,
    148741,
    149234,
    150078,
    151174,
    151702,
    152546,
    153662,
    154194,
    155059,
    156203,
    156783,
    157676,
    158828,
    159453,
    160394,
    161590,
    162179,
    163140,
    164371,
    165059,
    166036,
    167290,
    168002,
    169018,
    170302,
    171068,
    172112,
    173431,
    174252,
    175326,
    176682,
    177551,
    178678,
    180065,
    180983,
    182141,
    183575,
    184558,
    185754,
    187236,
    188275,
    189521,
    191083,
    192161,
    193429,
    195093,
    196325,
    197585,
    199242,
    200502,
    201917,
    203606,
    204979,
    206457,
    208199,
    209672,
    211208,
    213031,
    214604,
    216228,
    218097,
    219811,
    221518,
    223474,
    225323,
    227119,
    229181,
    231190,
    233087,
    235222,
    237404,
    239409,
    241670,
    244043,
    246171,
    248545,
    251138,
    253395,
    255897,
    258726,
    261138,
    263770,
    266923,
    269503,
    272309,
    275758,
    278571,
    281559,
    285440,
    288480,
    291698,
    296017,
    299345,
    302784,
    307645,
    311283,
    315036,
    320572,
    324627,
    328719,
    335050,
    339572,
    344090,
    351440,
    356589,
    361614,
    370166,
    376019,
    381711,
    391845,
    398647,
    405186,
    417438,
    425520,
    433171,
    448331,
    458163,
    467335,
    486632,
    498940,
    510296,
    535892,
    552144,
    567236,
    603551,
    626394,
    647583,
    703471,
    739917,
    774856,
    878584,
    951148,
    1028127,
    1330087,
    1618390,
    2252060,
};

auto i = 1U;
auto lastTestRevTime = 0UL;
auto elapsedTime = 0L;
auto i2 = 0U;

void simulateRotation()
{
    if (micros() - lastTestRevTime < test[i] || i2 == 3)
        return;

    lastTestRevTime = micros();
    i++;
    flywheelService.processRotation(elapsedTime + std::accumulate(test, test + i, 0));
    if (i >= std::size(test))
    {
        Log.infoln("Going to sleep. Processed number of rotations: %d", i);
        elapsedTime = std::accumulate(test, test + i, 0);
        i = 0;
        i2++;
    }
}