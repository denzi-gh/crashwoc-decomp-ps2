/*
 * Unit: game/text
 *
 * Functions:
 *   0x002409e8 NewLanguage
 *   0x00240b88 AddSpacesIntoText
 *   0x00240c48 JStrLen
 */

#include "game/gametypes.h"

extern struct game_s Game;
extern struct powerdata_s PData[6];

char *LanguageName[6] = {
    "ENGLISH",
    "FRANCAIS",
    "DEUTSCH",
    "ITALIANO",
    "ESPA\245OL",
    "NEDERLANDS"
};

char *tHIRAGANA[6] = { "HIRAGANA", "HIRAGANA", "HIRAGANA", "HIRAGANA", "HIRAGANA", "HIRAGANA" };

char *tKATAKANA[6] = { "KATAKANA", "KATAKANA", "KATAKANA", "KATAKANA", "KATAKANA", "KATAKANA" };

char *tABC[6] = { "ABC", "ABC", "ABC", "ABC", "ABC", "ABC" };

char *HubName[6][6] = {
    { "EARTH", "TERRE", "ERDE", "TERRA", "TIERRA", "AARDE" },
    { "WATER", "EAU", "WASSER", "ACQUA", "AGUA", "WATER" },
    { "FIRE", "FEU", "FEUER", "FUOCO", "FUEGO", "VUUR" },
    { "WEATHER", "METEO", "WETTER", "CLIMA", "CLIMA", "HET WEER" },
    { "SPACE", "ESPACE", "WELTRAUM", "SPAZIO", "ESPACIO", "DE RUIMTE" },
    { "BONUS", "BONUS", "BONUS", "BONUS", "BONUS", "BONUS" }
};

char *LevelName[44][6] = {
    {
        "ESKIMO ROLL",
        "GLACE ESKIMO",
        "ESKIMO-ROLLE",
        "IGLOO GLU-GLU",
        "ROLLO DE ESQUIMAL",
        "ESKIMOROL"
    },
    {
        "CRASH AND BURN",
        "TOUT FEU TOUT FLAMME",
        "CRASH UND FEUER",
        "AL FUOCO! AL FUOCO!",
        "CRASH A FUEGO LENTO",
        "CRASH VERBRANDT"
    },
    { "H2 OH NO", "ENTRE DEUX EAUX", "H2 OH NEIN", "H2O - OH NO!", "H2 OH NO", "H2 OH NO" },
    {
        "SMOKEY AND THE BANDICOOT",
        "PAS DE FUMEE SANS BANDICOOT",
        "SMOKEY UND DAS BANDICOOT",
        "SMOKEY E IL BANDICOOT",
        "BANDICOOT AHUMADO",
        "SMOKEY AND THE BANDICOOT"
    },
    {
        "COMPACTOR REACTOR",
        "REACTEUR REDUCTEUR",
        "KOMPAKTOR-REAKTOR",
        "REATTORE TRITURATORE",
        "REACTOR COMPRESOR",
        "COMPACTOR REACTOR"
    },
    { "AVALANCHE", "AVALANCHE", "LAWINE", "VALANGA", "AVALANCHA", "LAWINE" },
    {
        "CORAL CANYON",
        "LE CANYON DE CORAIL",
        "KORALLENRIFF",
        "CANYON DI CORALLO",
        "EL CA\245\242N DE CORAL",
        "CORAL CANYON"
    },
    {
        "ARCTIC ANTICS",
        "BANDICOOT ON ICE",
        "ARKTISCHE AKTIVIT\216TEN",
        "PARTI PER L'ARTICO",
        "BROMAS \240RTICAS",
        "ARCTISCHE ACTIVITEITEN"
    },
    {
        "JUNGLE RUMBLE",
        "LA JUNGLE EN FOLIE",
        "DSCHUNGELGERUMPEL",
        "IL RUGGITO DELLA GIUNGLA",
        "LA LLAMADA DE LA SELVA",
        "JUNGLE RUMBLE"
    },
    {
        "FORCE OF NATURE",
        "FORCE DE LA NATURE",
        "NATURGEWALTEN",
        "FORZA DELLA NATURA",
        "FUERZAS DE LA NATURALEZA",
        "DE KRACHT VAN DE NATUUR"
    },
    {
        "THE GAUNTLET",
        "LE DEFI",
        "SPIESSRUTEN LAUFEN",
        "IL BANDICOOT NEL MIRINO",
        "EL GUANTE",
        "THE GAUNTLET"
    },
    {
        "BAMBOOZLED",
        "LA GRANDE ENTOURLOUPE",
        "REINGELEGT",
        "ABBINDOLATO",
        "ATOLONDRADO",
        "BEETGENOMEN"
    },
    {
        "BANZAI BONSAI",
        "BANZAI BONSAI",
        "BANZAI BONSAI",
        "BANZAI BONSAI",
        "BONSAI BANZAI",
        "BANZAI BONSAI"
    },
    {
        "TORNADO ALLEY",
        "LA ROUTE DU VENT",
        "TORNADOBAHN",
        "VIA COL TORNADO",
        "\255TORNADO!",
        "TORNADO ALLEY"
    },
    { "TSUNAMI", "TSUNAMI", "TSUNAMI", "TSUNAMI", "TSUNAMI", "TSUNAMI" },
    {
        "FAHRENHEIT FRENZY",
        "CHAUD DEVANT",
        "FAHRENHEIT-VERWIRRUNG",
        "FOLLI FAHRENHEIT ",
        "ALTAS TEMPERATURAS",
        "FAHRENHEIT FRENZY"
    },
    {
        "MEDIEVAL MADNESS",
        "FOLIE MEDIEVIALE",
        "MITTELALTERLICHE VERR\232CKTHEIT",
        "QUESTO PAZZO, PAZZO MEDIOEVO",
        "LOCURA MEDIEVAL",
        "MEDIEVAL MADNESS"
    },
    {
        "WIZARDS AND LIZARDS",
        "Y A PAS DE LEZARD",
        "HEXENMEISTER UND ECHSEN",
        "DRAGHI E MAGHI",
        "MAGOS Y LAGARTOS",
        "TOVENAARS EN HAGEDISSEN"
    },
    {
        "THAT SINKING FEELING",
        "UN SIPHON FONT FONT ",
        "MIESE STIMMUNG",
        "MI SENTO GI\227...GI\227...GI\227",
        "ESTO SE HUNDE",
        "HELP, WE ZINKEN"
    },
    {
        "GOLD RUSH",
        "LA RUEE VERS L'OR",
        "GOLDFIEBER",
        "LA FEBBRE DELL'ORO",
        "FIEBRE DEL ORO",
        "GOUDKOORTS"
    },
    {
        "DROID VOID",
        "PANIQUE MECANIQUE",
        "DROIDEN-LEERE",
        "VUOTO COSMICO",
        "DROIDES VAC\241OS",
        "DROID VOID"
    },
    {
        "RUMBLE IN THE ROKS",
        "ROK A GOGO",
        "GROLLEN IM GEBIRGE",
        "SULLE ROCCIE",
        "PIEDRA SOBRE PIEDRA",
        "HET ROMMELT IN DE ROTSEN"
    },
    {
        "CRASHES TO ASHES",
        "CRASH EXPLOSIF",
        "CRASH WIRD ZU ASCHE",
        "PER UN PUGNO DI CENERE",
        "CRASH A LA PARRILLA",
        "CRASHES VERGAAT TOT AS"
    },
    {
        "DRAIN DAMAGE",
        "TOUT A L'EGOUT",
        "ROHRBRUCH",
        "GUASTO IDRAULICO",
        "NI GOTA, NI GOTA",
        "DRAIN DAMAGE"
    },
    {
        "ATMOSPHERIC PRESSURE",
        "PRESSION ATMOSPHERIQUE",
        "ATMOSPH\204RISCHER DRUCK",
        "PRESSIONE ATMOSFERICA",
        "PRESI\242N ATMOSF\202RICA",
        "ATMOSFERISCHE DRUK"
    },
    {
        "CRUNCH TIME",
        "MOMENT CRUNCHISIF",
        "CRUNCH-ZEIT",
        "TEMPO DI CRUNCH",
        "CRUNCH Y NEO",
        "CRUNCH VOOR NEO"
    },
    {
        "CRASHTEROIDS",
        "CRASHSTEROIDES",
        "CRASHSTEROIDEN",
        "CRASHTEROIDI",
        "CRASHTEROIDES",
        "CRASHSTEROIDEN"
    },
    {
        "KNIGHT TIME",
        "CHEVALIER DU SOIR",
        "RITTERZEIT",
        "E VIA A CAVALLO",
        "TIEMPO DE CABALLEROS",
        "KNIGHT TIME"
    },
    {
        "GHOST TOWN",
        "VILLE FANTOME",
        "GEISTERSTADT",
        "CITT\205 FANTASMA",
        "CIUDAD FANTASMA",
        "SPOOKSTAD"
    },
    {
        "ICE STATION BANDICOOT",
        "STATION GLACIAIRE BANDICOOT",
        "EISSTATION BANDICOOT",
        "BASE ARTICA BANDICOOT",
        "ESTACI\242N DE HIELO BANDICOOT",
        "IJSSTATION BANDICOOT"
    },
    {
        "WEATHERING HEIGHTS",
        "METEO DE HURLEVENT",
        "WETTERH\224HEN",
        "CIME TORMENTOSE",
        "ALTURAS BORRASCOSAS",
        "WEATHERING HEIGHTS"
    },
    {
        "CRATE BALLS OF FIRE",
        "LE FEU AUX CAISSES",
        "KISTEN UND FEUERB\204LLE",
        "SI ACCOMODI ALLA CASSA... ",
        "EL CAJ\242N DE LAS BOLAS DE FUEGO",
        "CRATE BALLS OF FIRE"
    },
    {
        "SOLAR BOWLER",
        "BOWLING SOLAIRE",
        "SONNEN-KEGELBAHN",
        "BOWLING SOLARE",
        "BOLOS SOLARES",
        "SOLAR BOWLER"
    },
    {
        "CORTEX VORTEX",
        "CORTEX VORTEX",
        "CORTEX-WIRBEL",
        "CORTEX VORTEX",
        "CORTEX EL TORBELLINO",
        "CORTEX VORTEX"
    },
    {
        "SEA SHELL SHENANIGANS",
        "LES DEDANS DE LA MER",
        "MUSCHELM\216TZCHEN",
        "FOLLIE ACQUATICHE",
        "MOLUSCOS TRAVIESOS",
        "SEA SHELL SHENANIGANS"
    },
    { "TITLES", "TITLES", "TITLES", "TITLES", "TITLES", "TITLES" },
    { "AIRSHIP", "AIRSHIP", "AIRSHIP", "AIRSHIP", "AIRSHIP", "AIRSHIP" },
    { "FRONT END", "FRONT END", "FRONT END", "FRONT END", "FRONT END", "FRONT END" },
    { "GAME OVER", "PERDU", "SPIEL BEENDET", "GAME OVER", "PARTIDA FINALIZADA", "AFGELOPEN" },
    { "INTRO", "INTRO", "INTRO", "INTRO", "INTRO", "INTRO" },
    { "SPACE", "SPACE", "SPACE", "SPACE", "SPACE", "SPACE" },
    { "OUTRO", "OUTRO", "OUTRO", "OUTRO", "OUTRO", "OUTRO" },
    { "TEST ZONE", "TEST ZONE", "TEST ZONE", "TEST ZONE", "TEST ZONE", "TEST ZONE" },
    { "CREDITS", "CREDITS", "CREDITS", "CREDITS", "CREDITS", "CREDITS" }
};

char *PlaceName[5][6] = {
    { "1ST", "1ER", "1.", "1\370", "1\370", "1E" },
    { "2ND", "2EME", "2.", "2\370", "2\370", "2E" },
    { "3RD", "3EME", "3.", "3\370", "3\370", "3E" },
    { "4TH", "4EME", "4.", "4\370", "4\370", "4E" },
    { "5TH", "5EME", "5.", "5\370", "5\370", "5E" }
};

char *PlaceName3[5][6] = {
    { "1ST", "1 :", "1. ", "1\370 ", "1\370 ", "1E " },
    { "2ND", "2 :", "2. ", "2\370 ", "2\370 ", "2E " },
    { "3RD", "3 :", "3. ", "3\370 ", "3\370 ", "3E " },
    { "4TH", "4 :", "4. ", "4\370 ", "4\370 ", "4E " },
    { "5TH", "5 :", "5. ", "5\370 ", "5\370 ", "5E " }
};

char *tNEWGAME[6] = {
    "NEW GAME",
    "NOUVELLE PARTIE",
    "NEUES SPIEL",
    "NUOVA PARTITA",
    "NUEVA PARTIDA",
    "NIEUW SPEL"
};

char *tDEMO[6] = { "DEMO", "DEMO", "DEMO", "DEMO", "DEMO", "DEMO" };

char *tBONUS[6] = { "BONUS", "BONUS", "BONUS", "BONUS", "BONO", "BONUS" };

char *tCHECK[6] = { "CHECK", "POINT", "KONTROLL-", "PUNTO", "PUESTO DE ", "CHECK" };

char *tPOINT[6] = { "POINT", "DE DEPART", "PUNKT", "DI CONTROLLO", "CONTROL", "PUNT" };

char *tENTERINGTIMETRIAL[6] = {
    "ENTERING TIME TRIAL",
    "DEBUT DU CONTRE LA MONTRE",
    "BEGINN DES ZEIT-MODUS",
    "PROVA A TEMPO",
    "EMPEZANDO CONTRARRELOJ",
    "RACE TEGEN DE TIJD BEGINNEN"
};

char *tRESUME[6] = { "RESUME", "REPRENDRE", "FORTSETZEN", "RIPRENDI", "REANUDAR", "HERVATTEN" };

char *tOPTIONS[6] = { "OPTIONS", "OPTIONS", "OPTIONEN", "OPZIONI", "OPCIONES", "OPTIES" };

char *tQUIT[6] = { "QUIT", "QUITTER", "BEENDEN", "ESCI", "ABANDONAR", "STOPPEN" };

char *tWARPROOM[6] = {
    "WARP ROOM",
    "SAS VIRTUEL",
    "ZEITSPRUNG-RAUM",
    "STANZA TELETR.",
    "SALA DEL TIEMPO",
    "ZAPKAMER"
};

char *tRESTARTTRIAL[6] = {
    "RESTART TRIAL",
    "REESSAYER CONTRE LA MONTRE",
    "NEUER VERSUCH",
    "RICOMINCIA PROVA A TEMPO",
    "CONTRARRELOJ NUEVA",
    "OPNIEUW BEGINNEN"
};

char *tRESTARTRACE[6] = {
    "RESTART RACE",
    "RECOMMENCER LA COURSE",
    "RENNEN WIEDERHOLEN",
    "RICOMINCIA LA GARA",
    "REINICIAR CARRERA",
    "RACE OPNIEUW BEGINNEN"
};

char *tBESTTIME[6] = {
    "BEST TIME",
    "MEILLEUR TEMPS",
    "BESTZEIT",
    "MIGLIOR TEMPO",
    "MEJOR TIEMPO",
    "BESTE TIJD"
};

char *t2NDBESTTIME[6] = {
    "2ND BEST TIME",
    "2EME MEILLEUR TEMPS",
    "ZWEITBESTE ZEIT",
    "2\370 MIGLIOR TEMPO",
    "MEJOR TIEMPO 2",
    "TWEEDE TIJD"
};

char *t3RDBESTTIME[6] = {
    "3RD BEST TIME",
    "3EME MEILLEUR TEMPS",
    "DRITTBESTE ZEIT",
    "3\370 MIGLIOR TEMPO",
    "MEJOR TIEMPO 3",
    "DERDE TIJD"
};

char *tNONEWTIME[6] = {
    "NO NEW TIME",
    "PAS DE NOUVEAU TEMPS",
    "KEINE NEUE ZEIT",
    "NESSUN NUOVO TEMPO",
    "NING\243N TIEMPO NUEVO",
    "GEEN NIEUWE TIJD"
};

char *tCONTINUE[6] = { "CONTINUE", "CONTINUER", "WEITER", "CONTINUA", "CONTINUAR", "DOORGAAN" };

char *tGAMEOVER[6] = {
    "GAME OVER",
    "PERDU",
    "SPIEL BEENDET",
    "FINE DEL GIOCO",
    "PARTIDA FINALIZADA",
    "SPEL AFGELOPEN"
};

char *tYES[6] = { "YES", "OUI", "JA", "SI", "S\241", "JA" };

char *tNO[6] = { "NO", "NON", "NEIN", "NO", "NO", "NEE" };

char *tON[6] = { "ON", "OUI", "EIN", "S\215", "S\241", "AAN" };

char *tOFF[6] = { "OFF", "NON", "AUS", "NO", "NO", "UIT" };

char *tGO[6] = { "GO", "PARTEZ", "LOS", "VAI", "ADELANTE", "START" };

char *tDONE[6] = { "DONE", "TERMINE", "FERTIG", "FATTO", "LISTO", "KLAAR" };

char *tCANCEL[6] = { "CANCEL", "ANNULER", "ABBRECHEN", "ANNULLA", "CANCELAR", "ANNULEREN" };

char *tEXIT[6] = { "EXIT", "QUITTER", "VERLASSEN", "ESCI", "SALIR", "AFSLUITEN" };

char *tVIBRATION[6] = {
    "VIBRATION",
    "VIBRATIONS",
    "VIBRATION",
    "VIBRAZIONE",
    "VIBRACI\242N",
    "VIBRATIE"
};

char *tSOUND[6] = { "SOUND", "SON", "SOUND", "SUONO", "SONIDO", "GELUID" };

char *tSOUNDOPTIONS[6] = {
    "SOUND OPTIONS",
    "OPTIONS SON",
    "SOUNDOPTIONEN",
    "OPZIONI SONORE",
    "OPCIONES SONIDO",
    "GELUIDSOPTIES"
};

char *tSURROUND[6] = { "SURROUND", "SURROUND", "SURROUND", "SURROUND", "ENVOLVENTE", "SURROUND" };

char *tSFXVOLUME[6] = {
    "FX VOLUME",
    "VOLUME SON",
    "SOUND-LAUTST\204RKE",
    "VOLUME SUONO",
    "VOLUMEN DEL SONIDO",
    "GELUIDSVOLUME"
};

char *tMUSICVOLUME[6] = {
    "MUSIC VOLUME",
    "VOLUME MUSIQUE",
    "MUSIK-LAUTST\204RKE",
    "VOLUME MUSICA",
    "VOLUMEN DE LA M\243SICA",
    "MUZIEKVOLUME"
};

char *tSTERO[6] = { "STEREO", "STEREO", "STEREO", "STEREO", "EST\220REO", "STEREO" };

char *tMONO[6] = { "MONO", "MONO", "MONO", "MONO", "MONO", "MONO" };

char *tLANGUAGE[6] = { "LANGUAGE", "LANGUE", "SPRACHE", "LINGUA", "IDIOMA", "TAAL" };

char *tRELICSREQUIRED[6] = {
    "%i RELICS REQUIRED",
    "%i RELIQUES NECESSAIRES",
    "%i RELIKTE BEN\231TIGT",
    "%i RELIQUIE NECESSARI",
    "%i RELIQUIAS SOLDADOS",
    "%i RELIKWIE\211N NODIG"
};

char *tCRYSTALS[6] = {
    "CRYSTALS",
    "CRISTAUX",
    "KRISTALLE",
    "CRISTALLI",
    "CRISTALES",
    "KRISTALLEN"
};

char *tGEMS[6] = { "GEMS", "GEMMES", "EDELSTEINE", "GEMME", "GEMAS", "EDELSTENEN" };

char *tRELICS[6] = { "RELICS", "RELIQUES", "RELIKTE", "RELIQUIE", "RELIQUIAS", "RELIKWIE\211N" };

char *tTIMES[6] = { "TIMES", "TEMPS", "ZEITEN", "TEMPI", "TIEMPOS", "TIJDEN" };

char *tPOWERS[6] = { "POWERS", "POUVOIRS", "KR\216FTE", "POTERI", "PODERES", "VERMOGEN" };

char *tSUPERBELLYFLOP[6] = {
    "SUPER BELLY FLOP",
    "SUPER ATTAQUE SAUT PUIS PLAT",
    "TURBO-BODENWURF",
    "SUPER PANCIATA",
    "S\243PER BARRIGAZO",
    "SUPER CHARGED BODY SLAM"
};

char *tSUPERBELLYFLOPTEXT[6][7] = {
    {
        "FOR A MORE POWERFUL BELLY FLOP",
        "PRESS x TO JUMP, AND THEN",
        "PRESS o AT THE",
        "TOP OF THE JUMP",
        "",
        "",
        ""
    },
    {
        "POUR PLUS DE PUISSANCE APPUIE",
        "SUR LA TOUCHE x POUR SAUTER,",
        "PUIS SUR LA TOUCHE o AU",
        "SOMMET DE TON SAUT",
        "",
        "",
        ""
    },
    {
        "DR\232CKEN SIE DIE x -TASTE F\232R",
        "EINEN H\216RTEREN BODENWURF VOR",
        "DEM SPRUNG, UND DIE o -TASTE",
        "WENN SIE AM H\224CHSTEN PUNKT",
        "DES SPRUNGS SIND",
        "",
        ""
    },
    {
        "PER UNA PANCIATA PI\227 POTENTE",
        "PREMI IL TASTO x PER SALTARE",
        "QUINDI PREMI IL TASTO o NEL",
        "PUNTO PI\227 ALTO DEL SALTO",
        "",
        "",
        ""
    },
    {
        "PARA ATIZAR BARRIGAZOS M\240S",
        "POTENTES PULSA x PARA SALTAR",
        "Y, A CONTINUACI\242N PULSA o",
        "CUANDO TE ENCUENTRES LO",
        "M\240S ALTO POSIBLE",
        "",
        ""
    },
    {
        "DRUK VOOR EEN NOG KRACHTIGERE",
        "BUIKLANDING OP DE x -TOETS OM",
        "TE SPRINGEN, DRUK VERVOLGENS",
        "OP DE o -TOETS ALS JE HET",
        "HOOGSTE PUNT VAN JE SPRONG",
        "BEREIKT HEBT",
        ""
    }
};

char *tDEATHTORNADOSPIN[6] = {
    "DEATH TORNADO SPIN",
    "TORNADE MORTELLE",
    "TODES-TORNADODREHUNG",
    "GIRAVOLTA TORNADO MORTALE",
    "GIRO MORTAL",
    "DEATH TORNADO SPIN"
};

char *tDEATHTORNADOSPINTEXT[6][7] = {
    {
        "TO SPIN LONGER AND FASTER",
        "OR TO GLIDE WHILE IN THE AIR",
        "PRESS s REPEATEDLY",
        "",
        "",
        "",
        ""
    },
    {
        "POUR TOURNER PLUS LONGTEMPS ET",
        "PLUS RAPIDEMENT, OU POUR PLANER",
        "UNE FOIS EN L'AIR, APPUIE",
        "PLUSIEURS FOIS SUR LA TOUCHE s",
        "",
        "",
        ""
    },
    {
        "DR\232CKEN SIE MEHRMALS DIE",
        "s -TASTE UM SICH L\216NGER UND",
        "SCHNELLER ZU DREHEN ODER IN",
        "DER LUFT ZU GLEITEN",
        "",
        "",
        ""
    },
    {
        "PER GIRARE PI\227 A LUNGO E PI\227",
        "VELOCEMENTE O PER SCIVOLARE",
        "MENTRE SEI IN ARIA PREMI",
        "RIPETUTAMENTE IL TASTO s",
        "",
        "",
        ""
    },
    {
        "PARA DAR UN GIRO M\240S R\240PIDO O",
        "M\240S LARGO O PARA PLANEAR",
        "MIENTRAS TE ENCUENTRAS EN",
        "EL AIRE PULSA EL",
        "BOT\242N s REPETIDAMENTE",
        "",
        ""
    },
    {
        "DRUK HERHAALDELIJK OP DE",
        "s -TOETS OM LANGER EN SNELLER",
        "ROND TE DRAAIEN TERWIJL JE",
        "IN DE LUCHT BENT",
        "",
        "",
        ""
    }
};

char *tDOUBLEJUMP[6] = {
    "DOUBLE JUMP",
    "DOUBLE SAUT",
    "DOPPELSPRUNG",
    "DOPPIO SALTO",
    "DOBLE SALTO",
    "DUBBELE SPRONG"
};

char *tDOUBLEJUMPTEXT[6][7] = {
    {
        "TO JUMP TWICE AS HIGH",
        "PRESS x TO JUMP, AND THEN",
        "PRESS x AGAIN AT THE",
        "TOP OF THE JUMP",
        "",
        "",
        ""
    },
    {
        "POUR SAUTER DEUX FOIS PLUS",
        "HAUT, APPUIE SUR LA TOUCHE x",
        "POUR SAUTER, PUIS AU SOMMET DE",
        "TON SAUT, APPUIE A NOUVEAU SUR",
        "CETTE TOUCHE",
        "",
        ""
    },
    {
        "SIE K\224NNEN DOPPELT SO HOCH",
        "SPRINGEN INDEM SIE VOR DEM",
        "ABSPRUNG UND AM H\224CHSTEN",
        "PUNKT JEWEILS x DR\232CKEN",
        "",
        "",
        ""
    },
    {
        "PER RAGGIUNGERE UN'ALTEZZA",
        "DOPPIA PREMI UNA PRIMA VOLTA",
        "IL TASTO x PER SALTARE QUINDI",
        "PREMILO DI NUOVO NEL PUNTO",
        "PI\227 ALTO DEL SALTO",
        "",
        ""
    },
    {
        "PARA SALTAR EL DOBLE DE ALTO,",
        "PULSA x PARA SALTAR Y A",
        "CONTINUACI\242N, VUELVE A PULSAR",
        "x CUANDO TE ENCUENTRES",
        "LO M\240S ALTO POSIBLE",
        "",
        ""
    },
    {
        "DRUK OP DE x -TOETS OM TWEE",
        "KEER ZO HOOG TE SPRINGEN DRUK",
        "VERVOLGENS WEER OP DE x -TOETS",
        "ALS JE HET HOOGSTE PUNT",
        "VAN JE SPRONG BEREIKT HEBT",
        "",
        ""
    }
};

char *tFRUITBAZOOKA[6] = { "FRUIT BAZOOKA", "BAZOOKA", "BAZOOKA", "BAZOOKA", "BAZUKA", "BAZOOKA" };

char *tFRUITBAZOOKATEXT[6][7] = {
    {
        "TO FIRE FRUIT AT A TARGET",
        "HOLD L1 TO RAISE THE GUN, AIM",
        "WITH THE DIRECTIONAL BUTTONS,",
        "THEN PRESS o TO FIRE",
        "",
        "",
        ""
    },
    {
        "POUR LANCER DES FRUITS SUR",
        "UNE CIBLE, LEVE LE CANON EN",
        "MAINTENANT LA TOUCHE L1",
        "ENFONCEE, VISE A L'AIDE DES",
        "TOUCHES DIRECTIONNELLES ET",
        "APPUIE SUR LA TOUCHE o",
        "POUR TIRER"
    },
    {
        "DR\232CKEN SIE DIE L1-TASTE UM",
        "DAS GEWEHR ANZUHEBEN, RICHTEN",
        "SIE ES MIT DEN RICHTUNGSTASTEN",
        "AUF DAS ZIEL AUS UND DR\232CKEN",
        "SIE DIE o -TASTE, UM MIT",
        "FR\232CHTEN AUF EIN ZIEL",
        "ZU SCHIE\341EN"
    },
    {
        "PER COLPIRE IL BERSAGLIO CON",
        "UN FRUTTO TIENI PREMUTO IL",
        "TASTO L1 PER SOLLEVARE IL",
        "BAZOOKA PRENDI LA MIRA CON I",
        "TASTI DIREZIONALI E PREMI IL",
        "TASTO o PER SPARARE",
        ""
    },
    {
        "PARA DISPARAR FRUTA, MANT\220N",
        "PULSADO L1 PARA LEVANTAR EL",
        "ARMA APUNTA UTILIZANDO LOS",
        "BOTONES DE DIRECC\241ON Y, A",
        "CONTINUACI\242N PULSA o",
        "PARA DISPARAR",
        ""
    },
    {
        "ALS JE FRUIT WILT AFVUREN,",
        "HOUD JE DE L1-TOETS INGEDRUKT",
        "OM AAN TE LEGGEN, RICHT JE",
        "D.M.V. DE RICHTINGSTOETSEN EN",
        "DRUK JE OP DE o -TOETS",
        "OM TE VUREN",
        ""
    }
};

char *tSPEEDSHOES[6] = {
    "SPEED SHOES",
    "SUPER SOULIERS",
    "RENNSCHUHE",
    "SCARPE TURBO",
    "ZAPATOS VELOCES",
    "SNELLE SCHOENEN"
};

char *tSPEEDSHOESTEXT[6][7] = {
    { "TO RUN FASTER", "HOLD R2 WHILE RUNNING", "", "", "", "", "" },
    { "POUR COURIR PLUS VITE", "MAINTIENS LA TOUCHE", "R2 ENFONCEE", "", "", "", "" },
    {
        "HALTEN SIE W\216HREND DES",
        "LAUFENS DIE R2-TASTE GEDR\232CKT",
        "UM SCHNELLER ZU WERDEN",
        "",
        "",
        "",
        ""
    },
    {
        "PER CORRERE PI\227 VELOCEMENTE",
        "TIENI PREMUTO IL TASTO R2",
        "DURANTE LA CORSA",
        "",
        "",
        "",
        ""
    },
    { "PARA CORRER M\240S R\240PIDO", "MANT\220N PULSADO R2", "MIENTRAS CORRES", "", "", "", "" },
    { "HOUD DE R2-TOETS INGEDRUKT", "OM SNELLER TE RENNEN", "", "", "", "", "" }
};

char *tSNEAKSHOES[6] = {
    "SNEAK SHOES",
    "MARCHER SUR LA POINTE DES PIEDS",
    "AUF ZEHENSPITZEN GEHEN",
    "AVANZARE IN PUNTA DI PIEDI",
    "DE PUNTILLAS",
    "OP JE TENEN"
};

char *tSNEAKSHOESTEXT[6][7] = {
    { "TO SNEAK ALONG SUSPENDED NITRO", "CRATES HOLD L2 WHILE MOVING", "", "", "", "", "" },
    {
        "POUR MARCHER SUR LA POINTE",
        "DES PIEDS MAINTIENS L2",
        "ENFONCEE PENDANT TA COURSE",
        "OU TA MARCHE",
        "",
        "",
        ""
    },
    {
        "HALTEN SIE W\216HREND DES LAUFENS",
        "ODER GEHENS DIE L2-TASTE",
        "GEDR\232CKT, UM SICH AUF",
        "ZEHENSPITZEN ZU BEWEGEN",
        "",
        "",
        ""
    },
    {
        "PER MUOVERTI IN PUNTA DI PIEDI",
        "TIENI PREMUTO IL TASTO L2",
        "MENTRE CAMMINI O CORRI",
        "",
        "",
        "",
        ""
    },
    {
        "PARA ANDAR DE PUNTILLAS,",
        "MANT\220N PULSADO L2 MIENTRAS",
        "CORRES O ANDAS",
        "",
        "",
        "",
        ""
    },
    {
        "HOUD DE L2-TOETS INGEDRUKT",
        "TERWIJL JE RENT OF LOOPT OM",
        "OP JE TENEN TE KUNNEN LOPEN",
        "",
        "",
        "",
        ""
    }
};

char *tPRESSxTOCONTINUE[6] = {
    "PRESS x TO CONTINUE",
    "POUR CONTINUER, APPUIE SUR LA TOUCHE x",
    "DR\232CKEN SIE ZUM WEITERSPIELEN x",
    "PREMI IL TASTO x PER CONTINUARE",
    "PULSA x PARA CONTINUAR",
    "DRUK OP DE x -TOETS OM DOOR TE GAAN"
};

char *tSELECTALEVELANDPRESSxTOPLAY[6] = {
    "SELECT A LEVEL AND PRESS x TO PLAY",
    "SELECT A LEVEL AND PRESS x TO PLAY",
    "SELECT A LEVEL AND PRESS x TO PLAY",
    "SELECT A LEVEL AND PRESS x TO PLAY",
    "SELECT A LEVEL AND PRESS x TO PLAY",
    "SELECT A LEVEL AND PRESS x TO PLAY"
};

char *tPRESSSTARTBUTTON[6] = {
    "PRESS START BUTTON",
    "APPUIE SUR START",
    "START-TASTE DR\232CKEN",
    "PREMI IL TASTO START",
    "PULSA EL BOT\242N START",
    "DRUK OP START"
};

char *tPRESSANYBUTTON[6] = {
    "PRESS ANY BUTTON",
    "APPUIE SUR UNE TOUCHE",
    "BELIEBIGE TASTE DR\232CKEN",
    "PREMI UN TASTO QUALSIASI",
    "PULSA CUALQUIER BOT\242N",
    "DRUK OP EEN WILLEKEURIGE KNOP"
};

char *tADJUSTSCREEN[6] = {
    "ADJUST SCREEN",
    "CENTRER L'ECRAN",
    "BILDSCHIRM",
    "REGOLA SCHERMO",
    "AJUSTES DE IMAGEN",
    "SCHERM AANPASSEN"
};

char *tHORIZONTAL[6] = {
    "HORIZONTAL",
    "HORIZONTAL",
    "HORIZONTAL",
    "ORIZZONTALE",
    "HORIZONTAL",
    "HORIZONTAAL"
};

char *tVERTICAL[6] = { "VERTICAL", "VERTICAL", "VERTIKAL", "VERTICALE", "VERTICAL", "VERTICAAL" };

char *tINSERTCONTROLLERINPORT1[6][2] = {
    { "INSERT CONTROLLER IN", "CONTROLLER PORT 1" },
    { "INSERER UNE MANETTE DANS", "LE PORT DE MANETTE 1" },
    { "CONTROLLER IN CONTROLLER-", "ANSCHLUSS EINSTECKEN" },
    { "INSERISCI CONTROLLER ", "NELL'INGRESSO CONTROLLER" },
    { "INSERTA EL CONTROLADOR", "EN EL PUERTO 1" },
    { "SLUIT CONTROLLER AAN", "OP CONTROLLER-AANSLUITING" }
};

char *tCHECKINGMEMORYCARD[6][2] = {
    { "CHECKING MEMORY CARD", "(8MB) (FOR p)" },
    { "VERIFICATION DE LA MEMORY", "CARD (8MO) (p)" },
    { "\232BERPR\232FE MEMORY CARD", "(8MB) (p)..." },
    { "CONTROLLO MEMORY CARD (8MB)", "(p) IN CORSO" },
    { "COMPROBANDO TARJETA DE", "MEMORIA (8MB) (p)" },
    { "GEHEUGENKAART WORDT GECONTROLEERD", "(8MB) (p)" }
};

char *tEMPTY[6] = { "EMPTY", "VIDE", "LEER", "VUOTO", "VAC\241O", "LEEG" };

char *tLOAD[6] = { "LOAD", "CHARGER", "LADEN", "CARICA", "CARGAR", "LADEN" };

char *tSAVE[6] = { "SAVE", "SAUVEGARDER", "SPEICHERN", "SALVA", "GUARDAR", "OPSLAAN" };

char *tDELETE[6] = { "DELETE", "SUPPRIMER", "L\224SCHEN", "CANCELLA", "BORRAR", "VERWIJDEREN" };

char *tLOADGAME[6] = {
    "LOAD GAME",
    "CHARGER UNE PARTIE",
    "SPIEL LADEN",
    "CARICA PARTITA",
    "CARGAR PARTIDA",
    "SPEL LADEN"
};

char *tSAVEGAME[6] = {
    "SAVE GAME",
    "SAUVEGARDER UNE PARTIE",
    "SPIEL SPEICHERN",
    "SALVA PARTITA",
    "GUARDAR PARTIDA",
    "SPEL OPSLAAN"
};

char *tDELETEGAME[6] = {
    "DELETE GAME",
    "SUPPRIMER UNE PARTIE",
    "SPIEL L\231SCHEN",
    "ELIMINA PARTITA",
    "BORRAR PARTIDA",
    "SPEL WISSEN"
};

char *tPLEASEINSERTMEMORYCARDSLOT1[6][3] = {
    { "PLEASE INSERT MEMORY CARD", "(8MB) (FOR p)", "IN MEMORY CARD SLOT 1" },
    { "INSERER UNE MEMORY CARD", "(8MO) (p)", "DANS LA FENTE N\3701" },
    { "BITTE MEMORY CARD", "(8MB) (p) IN MEMORY", "CARD-STECKPLATZ 1 EINLEGEN" },
    { "INSERISCI LA MEMORY CARD", "(8MB) (p)", "NELL'INGRESSO MEMORY CARD 1" },
    { "INSERTE LA TARJETA DE MEMORIA", "(8MB) (p)", "EN LA RANURA 1" },
    { "VOER GEHEUGENKAART", "(8MB) (p)", "IN AANSLUITING 1 IN" }
};

char *tNODATAAVAILABLE[6] = {
    "NO DATA AVAILABLE",
    "PAS DE DONNEES DISPONIBLES",
    "KEINE DATEN VERF\232GBAR",
    "NESSUN DATO DISPONIBILE",
    "NO HAY DATOS DISPONIBLES",
    "GEEN GEGEVENS BESCHIKBAAR"
};

char *tFORMAT[6] = { "FORMAT", "FORMATER", "FORMAT", "FORMATTA", "FORMATEAR", "FORMATTEREN" };

char *tOVERWRITE[6] = {
    "OVERWRITE",
    "ECRASER",
    "\232BERSCHREIBEN",
    "SOVRASCRIVI",
    "SOBRESCRIBIR",
    "OVERSCHRIJVEN"
};

char *tCONFIRMLOAD[6] = {
    "CONFIRM LOAD?",
    "CONFIRMER CHARGEMENT",
    "LADEN BEST\216TIGEN",
    "CONFERMA CARICAMENTO",
    "CONFIRMAR CARGAR",
    "LADEN BEVESTIGEN"
};

char *tCONFIRMSAVE[6] = {
    "CONFIRM SAVE?",
    "CONFIRMER SAUVEGARDE",
    "SPEICHERN BEST\216TIGEN",
    "CONFERMA SALVATAGGIO",
    "CONFIRMAR GRABAR",
    "OPSLAAN BEVESTIGEN"
};

char *tCONFIRMDELETE[6] = {
    "CONFIRM DELETE?",
    "CONFIRMER SUPPRESSION",
    "L\224SCHEN BEST\216TIGEN",
    "CONFERMA CANCELLAZIONE",
    "CONFIRMAR BORRAR",
    "VERWIJDEREN BEVESTIGEN"
};

char *tCONFIRMFORMAT[6] = {
    "CONFIRM FORMAT?",
    "CONFIRMER FORMATAGE",
    "FORMAT BEST\216TIGEN",
    "CONFERMA FORMATTAZIONE",
    "CONFIRMAR FORMATEAR",
    "FORMATTEREN BEVESTIGEN"
};

char *tCONFIRMOVERWRITE[6][2] = {
    { "DO YOU WISH TO", "OVERWRITE PREVIOUS DATA?" },
    { "CONFIRMER ECRASEMENT", "" },
    { "\232BERSCHREIBEN BEST\216TIGEN", "" },
    { "CONFERMA SOVRASCRITTURA", "" },
    { "CONFIRMAR SOBRESCRIBIR", "" },
    { "OVERSCHRIJVEN BEVESTIGEN", "" }
};

char *tLOADING[6] = {
    "LOADING",
    "CHARGEMENT",
    "LADE...",
    "CARICAMENTO IN CORSO",
    "CARGANDO",
    "BEZIG MET LADEN"
};

char *tSAVING[6] = {
    "SAVING",
    "SAUVEGARDE",
    "SPEICHERE...",
    "SALVATAGGIO IN CORSO",
    "GRABANDO",
    "BEZIG MET OPSLAAN"
};

char *tDELETING[6] = {
    "DELETING",
    "SUPPRESSION",
    "L\224SCHE...",
    "CANCELLAZIONE IN CORSO",
    "BORRANDO",
    "BEZIG MET VERWIJDEREN"
};

char *tFORMATTING[6] = {
    "FORMATTING",
    "FORMATAGE",
    "FORMATIERE...",
    "FORMATTAZIONE IN CORSO",
    "FORMATEANDO",
    "BEZIG MET FORMATTEREN"
};

char *tGAMELOADED[6] = {
    "GAME LOADED",
    "PARTIE CHARGEE",
    "SPIEL GELADEN",
    "PARTITA CARICATA",
    "PARTIDA CARGADA",
    "SPEL GELADEN"
};

char *tGAMESAVED[6] = {
    "GAME SAVED",
    "PARTIE SAUVEGARDEE",
    "SPIEL GESPEICHERT",
    "PARTITA SALVATA",
    "PARTIDA GRABADA",
    "SPEL OPGESLAGEN"
};

char *tGAMEDELETED[6] = {
    "GAME DELETED",
    "PARTIE SUPPRIMEE",
    "SPIEL GEL\224SCHT",
    "PARTITA CANCELLATA",
    "PARTIDA BORRADA",
    "SPEL VERWIJDERD"
};

char *tFAILEDTOLOAD[6] = {
    "FAILED TO LOAD",
    "ECHEC DU CHARGEMENT",
    "FEHLER BEIM LADEN",
    "IMPOSSIBILE CARICARE",
    "ERROR AL CARGAR",
    "LADEN MISLUKT"
};

char *tFAILEDTOSAVE[6] = {
    "FAILED TO SAVE",
    "ECHEC DE LA SAUVEGARDE",
    "FEHLER BEIM SPEICHERN",
    "IMPOSSIBILE SALVARE",
    "ERROR AL GRABAR",
    "OPSLAAN MISLUKT"
};

char *tFAILEDTODELETE[6] = {
    "FAILED TO DELETE",
    "ECHEC DE LA SUPPRESSION",
    "FEHLER BEIM L\224SCHEN",
    "IMPOSSIBILE CANCELLARE",
    "ERROR AL BORRAR",
    "VERWIJDEREN MISLUKT"
};

char *tFAILEDTOFORMAT[6] = {
    "FAILED TO FORMAT",
    "ECHEC DU FORMATAGE",
    "FEHLER BEIM FORMATIEREN",
    "IMPOSSIBILE FORMATTARE",
    "ERROR AL FORMATEAR",
    "FORMATTEREN MISLUKT"
};

char *tMEMORYCARDISNOTFORMATTED[6][3] = {
    { "MEMORY CARD", "(8MB) (FOR p)", "IS NOT FORMATTED" },
    { "LA MEMORY CARD (8MO) (p)", "INSEREE DANS LA FENTE N\3701", "N'EST PAS FORMATEE" },
    { "MEMORY CARD (8MB) (p)", "IN STECKPLATZ 1", "IST NICHT FORMATIERT" },
    { "LA MEMORY CARD (8MB) (p)", "NELL'INGRESSO MEMORY CARD 1", "NON \212 FORMATTATA" },
    { "LA TARJETA DE MEMORIA", "(8MB) (p) DE LA", "RANURA 1 NO EST\240FORMATEADA" },
    { "DE GEHEUGENKAART (8MB)", "(p) IN GEHEUGENKAART-", "AANSLUITING 1 IS NIET GEFORMATTEERD" }
};

char *tNOTENOUGHSPACEONMEMORYCARD[6][3] = {
    { "NOT ENOUGH SPACE ON", "MEMORY CARD", "(8MB) (FOR p)" },
    { "ESPACE INSUFFISANT SUR LA MEMORY", "CARD (8MO) (p) POUR", "SAUVEGARDER LA PARTIE" },
    { "NICHT GEN\232GEND PLATZ AUF MEMORY", "CARD (8MB) (p) ZUM", "SPEICHERN DES SPIELS" },
    { "SPAZIO INSUFFICIENTE SULLA MEMORY", "CARD (8MB) (p) PER", "SALVARE LA PARTITA" },
    { "NO HAY ESPACIO SUFICIENTE EN LA", "TARJETA DE MEMORIA (8MB) (p)", "PARA GRABAR LA PARTIDA" },
    { "ONVOLDOENDE RUIMTE OP DE", "GEHEUGENKAART (8MB) (p)", "OM SPEL OP TE SLAAN" }
};

char *tGAMEREQUIRESiKB[6] = {
    "GAME REQUIRES %iKB",
    "CRASH BANDICOOT VDC NECESSITE %iKO",
    "CRASH BANDICOOT WOC ERFORDERT %iKB",
    "CRASH BANDICOOT WOC RICHIEDE %iKB",
    "CRASH BANDICOOT VDC REQUIERE %iKB",
    "CRASH BANDICOOT WOC VEREIST %iKB"
};

char *tFORMATTINGCOMPLETE[6] = {
    "FORMATTING COMPLETE",
    "FORMATAGE TERMINE",
    "FORMATIERUNG BEENDET",
    "FORMATTAZIONE COMPLETATA",
    "FORMATEO FINALIZADO",
    "FORMATTEREN VOLTOOID"
};

char *tDONOTREMOVEMEMORYCARD[6][3] = {
    { "PLEASE DO NOT REMOVE MEMORY", "CARD (8MB) (FOR p)", "OR SWITCH OFF POWER" },
    { "NE PAS RETIRER LA MEMORY", "CARD (8MO) (p)", "NI ETEINDRE LA CONSOLE" },
    { "BITTE MEMORY CARD (8MB)", "(p) NICHT ENTFERNEN", "UND STROM NICHT AUSSCHALTEN" },
    { "NON RIMUOVERE MEMORY CARD", "(8MB) (p)", "E NON SPEGNERE CONSOLE" },
    { "NO RETIRE LA TARJETA DE", "MEMORIA (8MB) (p)", "NI CORTE LA ALIMENTACI\242N" },
    { "VERWIJDER GEHEUGENKAART", "(8MB) (p) NIET EN", "SCHAKEL DE STROOM NIET UIT" }
};

char *tYOUNEEDAMEMORYCARD[6][8] = {
    {
        "YOU NEED A MEMORY CARD",
        "(8MB) (FOR p)",
        "WITH AT LEAST %iKB OF FREE SPACE TO",
        "SAVE YOUR PROGRESS",
        "PLEASE INSERT A MEMORY CARD",
        "(8MB) (FOR p)",
        "WITH AT LEAST %iKB OR YOU WILL",
        "BE UNABLE TO SAVE"
    },
    {
        "NECESSITE CARTE MEMOIRE",
        "(8MO) (p)",
        "AVEC AU MOINS %iKO D'ESPACE LIBRE",
        "POUR SAUVEGARDER LA PARTIE",
        "VEUILLEZ INSERER CARTE MEMOIRE",
        "(8MO) (p)",
        "AVEC AU MOINS %iKO SINON IL SERA",
        "IMPOSSIBLE DE SAUVEG"
    },
    {
        "SIE BEN\224TIGEN EINE MEMORY CARD",
        "(8MB) (p)",
        "MIT MIND. %iKB FREIEM SPEICHER, UM IHRE",
        "SPIELDATEN ZU SPEICHERN",
        "SETZEN SIE DAHER F\232R DEN SPEICHERVORGANG",
        "EINE MEMORY CARD",
        "(8MB) (p)",
        "MIT MIND. %iKB EIN"
    },
    {
        "USA UNA MEMORY CARD",
        "(8MB) (p)",
        "CON ALMENO %iKB DI SPAZIO LIBERO PER",
        "SALVARE PROGRESSI",
        "INSERISCI UNA MEMORY CARD",
        "(8MB) (p)",
        "CON ALMENO %iKB PER",
        "POTER SALVARE"
    },
    {
        "USA UNA TARJETA DE MEMORIA",
        "(8MB) (p)",
        "CON %iKB DE ESPACIO LIBRE PARA",
        "GRABAR TU PROGRESO",
        "INTRODUCE UNA TARJETA DE MEMORIA",
        "(8MB) (p)",
        "CON AL MENOS %iKB O",
        "NO PODR\240S GRABAR"
    },
    {
        "ER IS EEN MEMORY CARD",
        "(8MB) (p)",
        "MET MINSTENS %iKB RUIMTE NODIG OM",
        "JE GAME OP TE SLAAN",
        "PLAATS EEN MEMORY CARD",
        "(8MB) (p)",
        "MET MINSTENS %iKB OF JE KUNT",
        "NIET OPSLAAN"
    }
};

extern void NuStrCpy(char *dst, char *src);
extern void DefaultTimeTrialNames(s32 all);
extern s32 CombinationCharacterBD(char c0, char c1);
extern s32 CombinationCharacterBC(char c0, char c1);


void NewLanguage(s32 lang) {
    Game.language = lang;
    PData[0].name = tSNEAKSHOES[Game.language];
    PData[0].text = tSNEAKSHOESTEXT[Game.language];
    PData[1].name = tDOUBLEJUMP[Game.language];
    PData[1].text = tDOUBLEJUMPTEXT[Game.language];
    PData[2].name = tDEATHTORNADOSPIN[Game.language];
    PData[2].text = tDEATHTORNADOSPINTEXT[Game.language];
    PData[3].name = tFRUITBAZOOKA[Game.language];
    PData[3].text = tFRUITBAZOOKATEXT[Game.language];
    PData[4].name = tSPEEDSHOES[Game.language];
    PData[4].text = tSPEEDSHOESTEXT[Game.language];
    PData[5].name = tSUPERBELLYFLOP[Game.language];
    PData[5].text = tSUPERBELLYFLOPTEXT[Game.language];
    DefaultTimeTrialNames(0);
}


void AddSpacesIntoText(char *text, u32 flags) {
    char buf[256];
    s32 i;
    s32 len;

    if (Game.language == 'c') {
        len = 0;
        for (i = 0; text[i] != 0; i++) {
            buf[len++] = text[i];
            if ((flags & 1) || ((flags & 2) && text[i] >= '0' && text[i] <= '9')
                || ((flags & 4) && text[i] == '-')) {
                buf[len++] = ' ';
            }
        }
        buf[len] = 0;
        NuStrCpy(text, buf);
    }
}


s32 JStrLen(char *text) {
    s32 len = 0;

    while (text[0] != 0 && text[1] != 0) {
        if (text[2] == 'B'
            && ((text[3] == 'D' && CombinationCharacterBD(text[0], text[1]))
                || (text[3] == 'C' && CombinationCharacterBC(text[0], text[1])))) {
            text += 4;
        } else {
            text += 2;
        }
        len++;
    }
    return len;
}
