typedef struct st_8027519_2 {
    unsigned int field_0;
    char padding_4[8];
    unsigned int field_c;
    unsigned int field_10;
} st_8027519_2;

typedef struct st_8027519_4 {
    char padding_0[4];
    unsigned int field_4;
} st_8027519_4;

typedef struct st_8027519_3 {
    float field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_8027519_3;

extern unsigned int g_803ef50[4];
extern st_8027519_3 g_8044d10;
extern unsigned int g_8045e10[4];
extern unsigned int g_80462b0[4];
extern unsigned int g_20001040;
extern unsigned int g_2000104c;
extern unsigned int g_20001054;
extern unsigned int g_200012e0;
extern unsigned int g_20021314;
extern unsigned int g_20021318;
extern unsigned int g_2002131c;
extern unsigned int g_20021320;
extern unsigned int g_20021328;
extern unsigned int g_2002133c;
extern unsigned int g_20021394;
extern unsigned int g_200213a8;
extern unsigned int g_200213ac;
extern unsigned int g_200213b4;
extern unsigned int g_200213b8;
extern unsigned int g_200213c0;
extern unsigned int g_20021c6c;
extern unsigned int g_20021c74;
extern unsigned int g_20021c78;
extern unsigned int g_20021c7c;
extern unsigned int g_20021c88;
extern unsigned int g_20021c90;
extern unsigned int g_20021c94;
extern unsigned int g_20021c9c;
extern unsigned int g_20021ca8;
extern unsigned short g_20021cac;
extern unsigned int g_20021cb0;
extern unsigned int g_20021cb4;
extern unsigned int g_20021cb8;
extern unsigned int g_20021cbc;
extern float g_20021cc0;
extern unsigned int g_20021dc0;
extern unsigned int g_20021de4;
extern unsigned int g_20021de8;
extern unsigned int g_20021df4;
extern unsigned int g_20021e08;
extern unsigned int g_20021e0c;
extern unsigned int g_20021e14;
extern unsigned int g_20021e18;
extern unsigned int g_20021e20;
extern unsigned int g_20021e64;
extern unsigned int g_20021e84;
extern unsigned int g_20021e88[4];
extern unsigned int g_20021ea8[4];
extern unsigned int g_20021f48;
extern unsigned int g_20021f4c[4];
extern unsigned int g_20021f70;
extern float g_20021f90;
extern unsigned int g_20021fb8;
extern unsigned int g_20021fd8[4];
extern unsigned int g_20021ff8[4];
extern unsigned int g_20022038;
extern unsigned int g_2002205c;
extern unsigned int g_20022064;
extern unsigned int g_20022068;
extern unsigned int g_20022070;
extern unsigned int g_20022074;
extern unsigned int g_20022078;
extern unsigned int g_2002207c;
extern unsigned int g_20022080;
extern unsigned int g_20022084;
extern unsigned int g_20022090;
extern float g_20022094;
extern unsigned int g_2002209c;
extern unsigned int g_200220a0;
extern unsigned int g_200220a4;
extern unsigned int g_200220a8;
extern unsigned int g_200220ac;
extern unsigned int g_200220b0;
extern unsigned int g_200220b4;
extern unsigned int g_200220b8;
extern unsigned int g_200220bc;
extern unsigned int g_200220c0;
extern unsigned int g_200220c4;
extern float g_200220c8;
extern unsigned int g_200220d4;
extern unsigned int g_200220dc;
extern unsigned int g_200220e0;
extern unsigned int g_200220e8;
extern unsigned int g_200220ec;
extern unsigned int g_200220f8;
extern unsigned int g_20022108;
extern unsigned int g_2002210c;
extern unsigned int g_20022114;
extern unsigned int g_20022140;
extern unsigned int g_20022144;
extern unsigned int g_20022148;
extern unsigned int g_2002214c;
extern unsigned int g_20022154;
extern unsigned int g_20022158;
extern unsigned int g_20022610;
extern unsigned int g_20022614;
extern unsigned int g_200230fc;
extern unsigned int g_2002409c;
extern unsigned int g_2002411c;
extern unsigned int g_20024120;
extern unsigned int g_20024124;
extern unsigned int g_20024128;
extern unsigned int g_2002412c;
extern unsigned int g_20024130;
extern unsigned int g_20024134;
extern unsigned int g_20024138;
extern unsigned int g_e0001004;

void sub_8027519(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v87;  // r3
    unsigned int v88;  // r7
    unsigned int v97;  // r2
    unsigned int v187;  // s27
    unsigned int v188;  // s11
    unsigned int v189;  // s9
    unsigned int v190;  // fpscr
    unsigned int v191;  // fpscr
    unsigned int v192;  // r6
    unsigned int v193;  // r4
    unsigned int v194;  // s6
    unsigned int v195;  // r0
    unsigned int v196;  // s19
    unsigned int v98;  // s4
    st_8027519_2 *v197;  // r6
    unsigned int v198;  // s15
    unsigned int v199;  // s14
    unsigned int v200;  // s21
    unsigned int v201;  // s22
    unsigned int v202;  // s26
    unsigned int v203;  // s4
    unsigned int v204;  // fpscr
    unsigned int v205;  // s27
    unsigned int v206;  // fpscr
    unsigned int v99;  // fpscr
    unsigned int v207;  // s4
    unsigned int v208;  // s3
    unsigned int v209;  // fpscr
    unsigned int v210;  // fpscr
    unsigned int v211;  // s18
    unsigned int v212;  // s4
    unsigned int v213;  // s3
    unsigned int v214;  // r3
    unsigned int v215;  // s14
    unsigned int v216;  // s13
    unsigned int v100;  // fpscr
    unsigned int v217;  // fpscr
    unsigned int v218;  // fpscr
    unsigned int v219;  // s3
    unsigned int v220;  // s21
    unsigned int v221;  // s18
    unsigned int v222;  // s27
    unsigned int v223;  // s30
    unsigned int v224;  // s24
    unsigned int v225;  // s0
    unsigned int *idx;  // r6
    unsigned int v101;  // s7
    unsigned int v227;  // s17
    unsigned int v228;  // s13
    unsigned int v229;  // s19
    unsigned int v230;  // s21
    unsigned int v231;  // s7
    unsigned int v232;  // s22
    unsigned int v233;  // s12
    unsigned int v234;  // s16
    unsigned int v235;  // s25
    unsigned int v236;  // s26
    unsigned int v102;  // fpscr
    unsigned int v237;  // fpscr
    unsigned int v238;  // fpscr
    unsigned int v239;  // fpscr
    unsigned int v240;  // fpscr
    unsigned int v241;  // s10
    unsigned int v242;  // s5
    unsigned int v243;  // s5
    unsigned int v244;  // s13
    unsigned int v245;  // s17
    unsigned int v246;  // r7
    unsigned int v103;  // s17
    unsigned int v247;  // s1
    unsigned int v248;  // s1
    unsigned int v249;  // s12
    unsigned int v250;  // s2
    unsigned int v251;  // r6
    int v252;  // r3
    unsigned int v253;  // s29
    unsigned int v254;  // s8
    unsigned int idx1;  // r7
    unsigned int v256;  // s6
    int v104;  // r2
    unsigned int v257;  // s10
    unsigned int v258;  // s0
    unsigned int v259;  // fpscr
    unsigned int v260;  // s10
    unsigned int v261;  // fpscr
    unsigned int v262;  // s12
    unsigned int v263;  // s19
    unsigned int v264;  // fpscr
    unsigned int v265;  // r2
    unsigned int idx2;  // r0
    unsigned int v105;  // s19
    unsigned int v267;  // r6
    int v268;  // r3
    unsigned int v269;  // s21
    unsigned int v270;  // r3
    int v271;  // r7
    unsigned int v272;  // r7
    int v273;  // r4
    int v274;  // r7
    unsigned int v275;  // r3
    unsigned int v276;  // r7
    unsigned int v106;  // r1
    int v277;  // r5
    int v278;  // r3
    unsigned int v279;  // s9
    unsigned int v280;  // s22
    unsigned int v281;  // s6
    unsigned int v282;  // fpscr
    int v283;  // r3
    unsigned int v284;  // s8
    unsigned int v285;  // s17
    unsigned int v286;  // s7
    unsigned int v89;  // r2
    unsigned int v107;  // s21
    void* iter;  // r11
    unsigned int *iter1;  // r12
    unsigned int v289;  // s5
    unsigned int v290;  // s0
    int index;  // r2
    unsigned int v292;  // s4
    unsigned int *iter2;  // r10
    unsigned int v294;  // s15
    unsigned int v295;  // fpscr
    unsigned int v296;  // r1
    unsigned int v108;  // fpscr
    unsigned int v297;  // r6
    unsigned int v298;  // r5
    unsigned int v299;  // fpscr
    unsigned int v300;  // s10
    unsigned int v301;  // r3
    unsigned int v302;  // s20
    unsigned int v303;  // s1
    unsigned int v304;  // s10
    unsigned int v305;  // r8
    unsigned long v306;  // d10
    unsigned int v109;  // s21
    unsigned int v307;  // s9
    unsigned int v308;  // r7
    unsigned int v309;  // fpscr
    unsigned int v310;  // s1
    unsigned int v311;  // s14
    unsigned int v312;  // r1
    unsigned int v313;  // r9
    unsigned int v314;  // r8
    unsigned int v315;  // r7
    unsigned int v316;  // r4
    unsigned int v110;  // fpscr
    unsigned int v317;  // s2
    unsigned int v318;  // r9
    unsigned int v319;  // s11
    unsigned int v320;  // s14
    unsigned int v321;  // s7
    unsigned int v322;  // s10
    unsigned int v323;  // s13
    unsigned int v324;  // s30
    unsigned int v325;  // s20
    unsigned int v326;  // s2
    unsigned int v111;  // s21
    unsigned int v327;  // s14
    unsigned int v328;  // s1
    unsigned int v329;  // s13
    unsigned int v330;  // s20
    unsigned int *v331;  // r9
    unsigned int *v332;  // r8
    unsigned int v333;  // s12
    unsigned int v334;  // s9
    unsigned int v335;  // fpscr
    unsigned int v336;  // fpscr
    unsigned int v112;  // r2
    unsigned int v337;  // s20
    unsigned int v338;  // fpscr
    unsigned int v339;  // r8
    float v340;  // s10
    unsigned int v341;  // r4
    unsigned int v342;  // fpscr
    unsigned int *v343;  // r9
    unsigned int v344;  // s14
    unsigned int v345;  // s13
    unsigned int v346;  // r0
    unsigned int v113;  // r11
    unsigned int v347;  // fpscr
    void* v348;  // r0
    unsigned int v349;  // s15
    unsigned int v350;  // s10
    unsigned int v351;  // r7
    unsigned int v352;  // s20
    unsigned int v353;  // r3
    unsigned int v354;  // r3
    unsigned int v355;  // s15
    int v356;  // r1
    unsigned int v114;  // r0
    unsigned int v357;  // s29
    unsigned int v358;  // s5
    unsigned int v359;  // s6
    unsigned int v360;  // s26
    unsigned int v361;  // r3
    unsigned int v362;  // fpscr
    unsigned int v363;  // s21
    unsigned int v364;  // s21
    unsigned int v365;  // s16
    unsigned int v366;  // s19
    int v115;  // r0
    unsigned int v367;  // fpscr
    unsigned int v368;  // fpscr
    unsigned int v369;  // s16
    unsigned int v370;  // s19
    unsigned int v371;  // fpscr
    unsigned int v372;  // fpscr
    unsigned int v373;  // s14
    unsigned int v374;  // s13
    unsigned int v375;  // s3
    unsigned int v376;  // s4
    int v116;  // r0
    unsigned int v377;  // s30
    unsigned int v378;  // s22
    unsigned int v379;  // s15
    int v380;  // r1
    unsigned int v381;  // r7
    int v382;  // r0
    unsigned int v383;  // r12
    unsigned int v384;  // r2
    unsigned int v385;  // r1
    unsigned int v386;  // r0
    int v90;  // r3
    int v117;  // r0
    float *v387;  // r3
    unsigned int v388;  // s27
    unsigned int v389;  // fpscr
    unsigned int v390;  // s25
    unsigned int v391;  // s12
    unsigned int v392;  // fpscr
    float *v393;  // r1
    float v394;  // s10
    unsigned int v395;  // r7
    unsigned int v396;  // r2
    unsigned int v118;  // r7
    unsigned int v397;  // r7
    unsigned int v398;  // r4
    unsigned int v399;  // s29
    unsigned int v400;  // s0
    unsigned int v401;  // r0
    unsigned int v402;  // s4
    unsigned int v403;  // s14
    unsigned int v404;  // s15
    st_8027519_2 *v405;  // r6
    unsigned int v406;  // s27
    int v119;  // r6
    unsigned int v407;  // s19
    unsigned int v408;  // s7
    unsigned int v409;  // fpscr
    unsigned int v410;  // s8
    unsigned int v411;  // fpscr
    unsigned int v412;  // s5
    unsigned int v413;  // s7
    unsigned int v414;  // s6
    unsigned int v415;  // fpscr
    unsigned int v416;  // s18
    int v120;  // r0
    unsigned int v417;  // r3
    unsigned int v418;  // fpscr
    unsigned int v419;  // s7
    unsigned int v420;  // s6
    unsigned int v421;  // s14
    unsigned int v422;  // s20
    unsigned int v423;  // fpscr
    unsigned int v424;  // fpscr
    unsigned int v425;  // s6
    unsigned int v426;  // s15
    unsigned int v121;  // s16
    unsigned int v427;  // s18
    unsigned int v428;  // s9
    unsigned int v429;  // s30
    unsigned int v430;  // s22
    unsigned int v431;  // s2
    unsigned int *v432;  // r6
    unsigned int v433;  // s28
    unsigned int v434;  // s20
    unsigned int v435;  // s17
    unsigned int v436;  // s5
    unsigned int v122;  // fpscr
    unsigned int v437;  // s3
    unsigned int v438;  // s4
    unsigned int v439;  // s11
    unsigned int v440;  // s27
    unsigned int v441;  // s26
    unsigned int v442;  // s21
    unsigned int v443;  // fpscr
    unsigned int v444;  // fpscr
    unsigned int v445;  // fpscr
    unsigned int v446;  // fpscr
    unsigned int v123;  // s18
    unsigned int v447;  // s20
    unsigned int v448;  // s30
    unsigned int v449;  // s0
    unsigned int v450;  // s0
    unsigned int v451;  // s24
    unsigned int v452;  // s1
    unsigned int v453;  // r0
    unsigned int v454;  // s17
    unsigned int v455;  // s4
    unsigned int v456;  // s4
    unsigned int v124;  // s18
    unsigned int v457;  // s25
    int v458;  // r0
    int v459;  // r3
    unsigned int v460;  // s21
    unsigned int v461;  // s19
    unsigned int v462;  // r6
    unsigned int v463;  // s0
    unsigned int v464;  // s1
    unsigned int v465;  // s12
    unsigned int v466;  // fpscr
    unsigned int v125;  // r4
    unsigned int v467;  // s1
    unsigned int v468;  // fpscr
    unsigned int v469;  // s11
    unsigned int v470;  // s28
    unsigned int v471;  // fpscr
    unsigned int v472;  // r2
    unsigned int v473;  // r7
    unsigned int v474;  // r0
    int v475;  // r3
    unsigned int v476;  // s20
    unsigned int v126;  // s21
    unsigned int v477;  // r3
    int v478;  // r6
    int v479;  // r6
    int v480;  // r0
    int v481;  // r6
    int v482;  // r6
    unsigned int v483;  // r3
    int v484;  // r7
    int v485;  // r3
    unsigned int v486;  // s21
    int v91;  // r1
    unsigned int v127;  // fpscr
    unsigned int v487;  // s9
    unsigned int v488;  // s30
    unsigned int v489;  // fpscr
    int v490;  // r3
    unsigned int v491;  // s12
    unsigned int v492;  // s0
    unsigned int v493;  // s20
    unsigned int v494;  // r11
    int *v495;  // r0
    unsigned int v496;  // r12
    unsigned long v128;  // d10
    unsigned int v497;  // s19
    unsigned int v498;  // s8
    unsigned int v499;  // s3
    int j;  // r2
    unsigned int *v501;  // r10
    unsigned int v502;  // s15
    unsigned int v503;  // fpscr
    unsigned int v504;  // r9
    unsigned int v505;  // r6
    unsigned int v506;  // r5
    unsigned long long v129;  // d10
    unsigned int v507;  // fpscr
    unsigned int v508;  // s2
    unsigned int v509;  // r3
    unsigned int v510;  // s5
    unsigned int v511;  // s5
    unsigned int v512;  // r8
    unsigned int v513;  // s15
    unsigned int v514;  // r7
    unsigned int v515;  // fpscr
    unsigned int v516;  // s5
    unsigned int v130;  // 4234
    unsigned int v517;  // r4
    unsigned int v518;  // s14
    unsigned int v519;  // r1
    unsigned int v520;  // r8
    unsigned int v521;  // s14
    unsigned int v522;  // s5
    unsigned int v523;  // s11
    unsigned int v524;  // s4
    unsigned int v525;  // s14
    unsigned int v526;  // s2
    unsigned int v131;  // fpscr
    unsigned int v527;  // s14
    unsigned int *v528;  // r9
    unsigned int *v529;  // r8
    unsigned int v530;  // s11
    unsigned int v531;  // s4
    unsigned int v532;  // fpscr
    unsigned int v533;  // fpscr
    unsigned int v534;  // s14
    unsigned int v535;  // fpscr
    unsigned int v536;  // r8
    unsigned int v132;  // r1
    float v537;  // s5
    unsigned int v538;  // fpscr
    unsigned int *v539;  // r9
    unsigned int v540;  // s15
    unsigned int v541;  // s14
    unsigned int v542;  // r1
    unsigned int v543;  // fpscr
    void* v544;  // r4
    unsigned int v545;  // s15
    unsigned int v546;  // s5
    unsigned int v133;  // r7
    unsigned int v547;  // r1
    unsigned int v548;  // s14
    unsigned int v549;  // r3
    unsigned int v550;  // r3
    int *v551;  // r0
    unsigned int v552;  // s15
    int v553;  // r1
    unsigned int v554;  // s0
    unsigned int v555;  // s8
    unsigned int v556;  // s9
    int v134;  // r5
    unsigned int v557;  // s23
    int v558;  // r3
    unsigned int v559;  // fpscr
    unsigned int v560;  // s25
    unsigned int v561;  // s25
    unsigned int v562;  // s27
    unsigned int v563;  // s24
    unsigned int v564;  // fpscr
    unsigned int v565;  // fpscr
    unsigned int v566;  // s27
    int v135;  // r5
    unsigned int v567;  // s24
    unsigned int v568;  // fpscr
    unsigned int v569;  // fpscr
    unsigned int v570;  // s14
    unsigned int v571;  // s30
    unsigned int v572;  // s6
    unsigned int v573;  // s7
    unsigned int v574;  // s22
    unsigned int v575;  // s28
    unsigned int v576;  // s15
    unsigned int v136;  // r1
    int v577;  // r1
    int v578;  // r7
    int v579;  // r0
    unsigned int v580;  // r12
    unsigned int v581;  // r2
    unsigned int v582;  // r1
    int v583;  // r0
    float *v584;  // r3
    float v585;  // s26
    unsigned int v586;  // fpscr
    unsigned int v92;  // r3
    float v137;  // s14
    unsigned int v587;  // s16
    unsigned int v588;  // s11
    unsigned int v589;  // fpscr
    float *v590;  // r1
    float v591;  // s5
    unsigned int v592;  // r7
    unsigned int v593;  // r2
    unsigned int v594;  // r7
    unsigned int v138;  // s0
    unsigned int v139;  // s2
    unsigned int v140;  // s3
    float v141;  // s7
    unsigned int v142;  // s12
    unsigned int v143;  // s12
    unsigned int v144;  // fpscr
    unsigned int v145;  // s9
    unsigned int v146;  // s10
    unsigned int *v93;  // r4
    int v147;  // r1
    int v148;  // r1
    unsigned int v149;  // r3
    unsigned int v150;  // r11
    unsigned int v151;  // s29
    unsigned int v152;  // s30
    unsigned int v153;  // s31
    int v154;  // r4
    unsigned int v155;  // s31
    unsigned int v156;  // s15
    unsigned int v94;  // r2
    int v157;  // r6
    int v158;  // r6
    unsigned int v159;  // r4
    unsigned int v160;  // s10
    unsigned int v161;  // s9
    unsigned int v162;  // fpscr
    unsigned int v163;  // s15
    unsigned int v164;  // s17
    unsigned int v165;  // s11
    unsigned int v166;  // fpscr
    unsigned int v95;  // r0
    unsigned int v167;  // s11
    unsigned int v168;  // s13
    int v169;  // r0
    int v170;  // r0
    unsigned int v171;  // s2
    unsigned int v172;  // fpscr
    unsigned int v173;  // fpscr
    unsigned int v174;  // fpscr
    unsigned int v175;  // s3
    unsigned int v176;  // 4151
    unsigned int *v96;  // r2
    unsigned int v177;  // fpscr
    unsigned int v178;  // r3
    unsigned int v179;  // r0
    int v180;  // r7
    int v181;  // r5
    unsigned int v182;  // r1
    unsigned int v183;  // s3
    unsigned int v184;  // r2
    unsigned int v185;  // s27
    unsigned int v186;  // fpscr
    int *v0;  // [bp-0x1c8], Other Possible Types: unsigned int *, unsigned int
    unsigned int *node;  // [bp-0x1c4], Other Possible Types: unsigned int
    unsigned int *v2;  // [bp-0x1c0], Other Possible Types: st_8027519_4 *
    st_8027519_4 *v3;  // [bp-0x1bc], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x1b8]
    unsigned int v5;  // [bp-0x1b4]
    unsigned int v6;  // [bp-0x1b0]
    unsigned int v7;  // [bp-0x1ac]
    int v8;  // [bp-0x1a8], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x1a4]
    unsigned int v10;  // [bp-0x1a0]
    int v11;  // [bp-0x19c], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x198]
    int v13;  // [bp-0x194], Other Possible Types: unsigned int
    unsigned int v14;  // [bp-0x190]
    unsigned int v15;  // [bp-0x18c]
    unsigned int *v16;  // [bp-0x188], Other Possible Types: unsigned int
    unsigned int *v17;  // [bp-0x184]
    st_8027519_2 *v18;  // [bp-0x180], Other Possible Types: unsigned int
    float *v19;  // [bp-0x17c]
    float *v20;  // [bp-0x178], Other Possible Types: unsigned int
    unsigned int *v21;  // [bp-0x174]
    int v22;  // [bp-0x170], Other Possible Types: unsigned int
    int v23;  // [bp-0x16c], Other Possible Types: unsigned int
    unsigned int v24;  // [bp-0x168]
    unsigned int v25;  // [bp-0x164]
    unsigned int v26;  // [bp-0x160]
    unsigned int v27;  // [bp-0x15c]
    unsigned int v28;  // [bp-0x158]
    unsigned int v29;  // [bp-0x154]
    unsigned int *v30;  // [bp-0x150], Other Possible Types: unsigned int
    unsigned int *v31;  // [bp-0x14c]
    unsigned int *v32;  // [bp-0x148], Other Possible Types: unsigned int
    unsigned int *v33;  // [bp-0x144]
    unsigned int v34;  // [bp-0x140]
    unsigned int *v35;  // [bp-0x13c]
    unsigned int *v36;  // [bp-0x138]
    unsigned int v37;  // [bp-0x134]
    unsigned int *v38;  // [bp-0x130]
    unsigned int v39;  // [bp-0x12c]
    unsigned int *v40;  // [bp-0x128], Other Possible Types: unsigned int
    unsigned int v41;  // [bp-0x124]
    unsigned int v42;  // [bp-0x120]
    unsigned int *v43;  // [bp-0x11c], Other Possible Types: unsigned int
    int *v44;  // [bp-0x118], Other Possible Types: unsigned int *, unsigned int
    unsigned int *v45;  // [bp-0x114], Other Possible Types: unsigned int
    unsigned int v46;  // [bp-0x110]
    unsigned int v47;  // [bp-0x10c]
    float v48;  // [bp-0x108]
    float v49;  // [bp-0x104]
    unsigned int v50;  // [bp-0x100]
    unsigned int v51;  // [bp-0xfc]
    float v52;  // [bp-0xf0]
    unsigned int *v53;  // [bp-0xec], Other Possible Types: unsigned int
    unsigned int *v54;  // [bp-0xe8]
    unsigned int *v55;  // [bp-0xe4]
    unsigned int *v56;  // [bp-0xe0]
    unsigned int v57;  // [bp-0xdc]
    unsigned int *v58;  // [bp-0xd8]
    unsigned int v59;  // [bp-0xd4]
    unsigned int *v60;  // [bp-0xd0]
    unsigned int *v61;  // [bp-0xcc]
    unsigned int v62;  // [bp-0xc8]
    unsigned int *v63;  // [bp-0xc4]
    unsigned int *v64;  // [bp-0xc0]
    unsigned int v65;  // [bp-0xbc]
    unsigned int v66;  // [bp-0xb8]
    unsigned int v67;  // [bp-0xb4]
    unsigned int v68;  // [bp-0xb0]
    unsigned int v69;  // [bp-0xac]
    unsigned int v70;  // [bp-0xa8]
    unsigned int v71;  // [bp-0xa4]
    unsigned int v72;  // [bp-0xa0]
    unsigned int v73;  // [bp-0x9c]
    unsigned int v74;  // [bp-0x98]
    unsigned int *v75;  // [bp-0x94]
    unsigned int v76;  // [bp-0x90]
    unsigned int *v77;  // [bp-0x8c], Other Possible Types: unsigned int
    unsigned int v78;  // [bp-0x88]
    unsigned int v79;  // [bp-0x84]
    unsigned int v80;  // [bp-0x80]
    unsigned int v81;  // [bp-0x7c]
    unsigned int v82;  // [bp-0x78]
    unsigned int v83;  // [bp-0x74]
    unsigned int v84;  // [bp-0x70]
    unsigned int v85;  // [bp-0x6c]
    char v86;  // [bp-0x68]

    g_e0001004 = 0;
    g_20021e14 = 0;
    g_20021394 = g_20021394 + 1;
    v24 = *((int *)536875748);
    if (g_2002205c == 1)
        return;
    v87 = (&g_2002409c)[g_2002411c];
    v88 = g_200220b0;
    g_200220b0 = g_20021c74;
    v89 = g_20021c74 - v88;
    (&g_2002409c)[g_2002411c] = v89;
    v90 = g_20024124 - v87 + v89;
    g_2002411c = g_2002411c + 1 & 31;
    g_20024124 = v90;
    g_2002205c = 1;
    if (v90 > g_20024120 && v90 > 0)
    {
        g_20024120 = v90;
LABEL_8027591:
        if (!g_20021cb0)
            goto LABEL_8027631;
    }
    else
    {
        v91 = -(v90);
        if (v91 <= g_20024120)
            goto LABEL_8027591;
        g_20024120 = v91;
        if (!g_20021cb0)
        {
LABEL_8027631:
            g_20022144 = 0x3f800000;
        }
    }
    v92 = *((int *)537006904);
    if (!*((int *)0x20021338) && !g_20021cac && *((int *)0x40021810) * 0x200000 > 0x7fffffff)
    {
        v93 = g_20022068;
        v75 = &g_20021cb4;
        v94 = g_20021ca8 * 8;
        g_20021cb4 = v94;
        v95 = v94 - (char *)v93 + 64;
        g_20021cb4 = (v94 < 2400 ? 2400 : v94);
        g_20021cac = 1;
        *(v75) = v93;
        g_20022068 = v75;
        v96 = (v95 <= 128 ? v93 : v75);
        g_20021ca8 = 1;
        v76 = 1;
        goto LABEL_80275f7;
    }
    else
    {
        v97 = g_20021ca8 + 1;
        g_20021cac = (short)*((int *)0x40021810) & 0x400;
        g_20021ca8 = v97;
        v75 = &g_20021cb4;
        if (g_20021ca8 > 23999)
        {
            g_20021ca8 = 23999;
            g_20021cb4 = 0;
            g_20021cb0 = 0;
            g_20022038 = 0;
            v76 = 0;
        }
        else
        {
            v96 = g_20021cb4;
            v76 = 0;
LABEL_80275f7:
            if (v96)
            {
                g_20022144 = g_803ef50[v96 >> 5] * 0x45bb8000;
                g_20021cb0 = 1;
            }
        }
    }
    v98 = *(0x20021c68) * 965355262;
    v100 = v99 & 0xfffffff | ((((CmpF(v98, 1065762916) >> 5 & 3 | CmpF(v98, 1065762916) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v98, 1065762916) >> 5 & 3 | CmpF(v98, 1065762916) & 1) & (CmpF(v98, 1065762916) >> 5 & 3 | CmpF(v98, 1065762916) & 1) >> 1 & 1)) * 0x10000000;
    if (!((v100 & 0xf0000000) >> 30 & 1 | (v100 & 0xf0000000) >> 31 & 1 ^ (v100 & 0xf0000000) >> 28 & 1))
    {
        v49 = (float)1.0;
    }
    else
    {
        if (!1)
            goto LABEL_0x8028855;
        if (!1)
            goto LABEL_0x8028859;
        else
            goto LABEL_80276eb;
        v101 = v98 - 1028131956;
        v102 = v100 & 0xfffffff | ((((CmpF(v101, 0) >> 5 & 3 | CmpF(v101, 0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v101, 0) >> 5 & 3 | CmpF(v101, 0) & 1) & (CmpF(v101, 0) >> 5 & 3 | CmpF(v101, 0) & 1) >> 1 & 1)) * 0x10000000;
        v49 = (float)(((v102 & 0xf0000000) >> 3 ^ v102 & 0xf0000000) & 0x10000000 ? 0 : v101);
        v100 = v102;
    }
LABEL_80276eb:
    if (*(0x20021c70) >= g_20022080)
    {
        v103 = *((int *)0x20021c70) - g_20022080;
    }
    else
    {
        v104 = g_20022078 - *((int *)0x20021c70);
        g_20022080 = *((int *)0x20021c70);
        v103 = 0;
        g_200220a0 = 1.0 / v104;
    }
    if (g_20022078 >= *((int *)537009264))
    {
        v105 = g_200220a0;
    }
    else
    {
        v105 = 1.0 / v103;
        g_20022078 = *((int *)0x20021c70);
        g_200220a0 = v105;
    }
    v106 = g_200220ac;
    v107 = v103 * v105;
    v108 = v100 & 0xfffffff | ((((CmpF(v107, 0) >> 5 & 3 | CmpF(v107, 0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v107, 0) >> 5 & 3 | CmpF(v107, 0) & 1) & (CmpF(v107, 0) >> 5 & 3 | CmpF(v107, 0) & 1) >> 1 & 1)) * 0x10000000;
    v109 = (((v108 & 0xf0000000) >> 3 ^ v108 & 0xf0000000) & 0x10000000 ? 0 : v107);
    v110 = v108 & 0xfffffff | ((((CmpF(v109, 1.0) >> 5 & 3 | CmpF(v109, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v109, 1.0) >> 5 & 3 | CmpF(v109, 1.0) & 1) & (CmpF(v109, 1.0) >> 5 & 3 | CmpF(v109, 1.0) & 1) >> 1 & 1)) * 0x10000000;
    v111 = (v110 & 0xf0000000 & 0x20000000 && !(v110 & 0xf0000000 & 0x40000000) ? 1.0 : v109);
    v112 = *((int *)0x2002208c);
    if (!g_20021cbc && *(0x2002208c) > 0)
    {
        g_200220ac = *((int *)0x20021c70);
        v113 = *((int *)0x20021c80) + 100;
        g_200220a4 = *((int *)0x20021c68) + 100;
        g_200220a8 = g_20021c6c + 100;
        g_200220b0 = g_20021c74 + 100;
        v114 = g_20021c7c + 100;
        g_200220b4 = g_20021c78 + 100;
        g_200220b8 = v114;
        g_200220bc = v113;
        v77 = &g_20024128;
        v18 = 0x20021dec;
        g_200220c0 = *((int *)0x20021c84) + 100;
        g_20024128 = 480;
        g_20021df4 = v111;
        if (!*((int *)0x20021338))
            goto LABEL_80277b1;
        goto LABEL_80286cd;
    }
    if ((*((int *)0x20021c70) - v106 < 0 ? v106 - *((int *)0x20021c70) : *((int *)0x20021c70) - v106) > 8)
    {
        v18 = 0x20021dec;
        g_200220ac = *((int *)0x20021c70);
        g_20021df4 = v111;
        if (*((int *)0x20021338))
        {
            v77 = &g_20024128;
LABEL_80286cd:
            if (*((int *)0x20021338) == 1)
            {
                v51 = &g_20001040;
                v115 = (*((int *)0x200012a0) + 1) * (v111 - 981668463) + 1;
                v116 = (v115 < 1 ? 1 : v115);
                if (g_20001040 != v116)
                {
                    g_20001040 = v116;
                    v117 = (*((int *)536875660) <= v116 ? *((int *)0x2000128c) : v116);
                    g_20001040 = *((int *)0x2000128c);
                    if (v117 > *((int *)536875680))
                    {
                        v116 = 1000;
                        *((unsigned int *)v51) = 1000;
                        goto LABEL_802888f;
                    }
                    else
                    {
                        g_20022074 = g_20022074 | 0x1000;
                        v116 = v117;
                        goto LABEL_802888f;
                    }
                }
            }
        }
        else
        {
            v77 = &g_20024128;
            if (0 < *((int *)537010316))
            {
LABEL_80277b1:
                g_20021cbc = *(0x2002208c) * (v111 - 981668463) + 1;
                goto LABEL_80277d3;
            }
            else if (*((int *)0x2002208c) != 1)
            {
LABEL_80277db:
                v51 = &g_20001040;
                v92 = *((int *)0x20021338);
                v116 = g_20001040;
                if (*((int *)0x2002208c))
                    goto LABEL_80288b5;
                goto LABEL_80277e9;
            }
            else
            {
LABEL_802aa4f:
                v51 = &g_20001040;
                g_20021cbc = *((int *)0x2002208c);
                v116 = g_20001040;
                v92 = *((int *)0x20021338);
                v118 = 0;
LABEL_80288bb:
                v12 = &g_200230fc;
                (&g_200230fc)[v112] = *((int *)(537010524 + v112 * 4)) - *((int *)(537010524 + v118 * 4));
                if (v116 != 1000 || v92)
                {
LABEL_8027801:
                    if (v92 != 1)
                    {
                        v119 = g_20021c74;
                        v120 = g_20021c7c;
                    }
                    else
                    {
                        v120 = 0;
                        v119 = 3247;
                        g_20021c74 = 3247;
                        g_20021c6c = 381;
                        g_20021c78 = 0;
                        g_20021c7c = 0;
                        g_200220b8 = 0;
                    }
                }
                else
                {
                    v119 = g_20021c74;
                    v120 = g_20021c7c;
                    g_20022074 = g_20022074 | 0x1000;
                }
            }
        }
    }
    else
    {
LABEL_802887b:
        if (*((int *)0x20021338) == 1)
        {
            v77 = &g_20024128;
            v116 = g_20001040;
            v51 = &g_20001040;
            v18 = 0x20021dec;
LABEL_802888f:
            g_20022614 = (384000 <= *((int *)(537010524 + *((int *)0x2002208c) * 4)) ? 384000 : *((int *)(537010524 + *((int *)0x2002208c) * 4)));
            g_20022610 = 0;
            g_20021cbc = 302;
            if (*((int *)0x2002208c))
            {
LABEL_80288b5:
                v112 = g_20021cbc;
                v118 = g_20021cbc - 1;
                goto LABEL_80288bb;
            }
LABEL_80277e9:
            v12 = &g_200230fc;
            g_20021cbc = *((int *)0x2002208c);
            g_200230fc = 0;
            goto LABEL_8027801;
        }
        else
        {
            v77 = &g_20024128;
            v18 = 537009644;
            if (!1)
                goto LABEL_802aa6d;
            else
                goto LABEL_80277d3;
        }
LABEL_80277d3:
        if (*((int *)0x2002208c) == 1)
            goto LABEL_802aa4f;
        goto LABEL_80277db;
    }
    v121 = g_20021e64 + *((int *)0x20021e68) + *((int *)0x20021e6c) + *((int *)0x20021e70);
    v122 = v110 & 0xfffffff | ((((CmpF(v121, 0.0) >> 5 & 3 | CmpF(v121, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v121, 0.0) >> 5 & 3 | CmpF(v121, 0.0) & 1) & (CmpF(v121, 0.0) >> 5 & 3 | CmpF(v121, 0.0) & 1) >> 1 & 1)) * 0x10000000;
    if (!((v122 & 0xf0000000) >> 30 & 1 | (v122 & 0xf0000000) >> 31 & 1 ^ (v122 & 0xf0000000) >> 28 & 1))
    {
        v122 = v122 & 0xfffffff | ((((CmpF(v121, 5.0) >> 5 & 3 | CmpF(v121, 5.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v121, 5.0) >> 5 & 3 | CmpF(v121, 5.0) & 1) & (CmpF(v121, 5.0) >> 5 & 3 | CmpF(v121, 5.0) & 1) >> 1 & 1)) * 0x10000000;
        if ((v122 & 0xf0000000) >> 31 & 1)
            v112 = (&g_20021fb8)[g_20021cb8];
    }
    g_20021de4 = v112;
    if (v119 < *((int *)536875668) || (v123 = (unsigned int)((int)(long long)(v119 - *((int *)0x20001294)) * *((int *)0x20021410)), v122 = v122 & 0xfffffff | ((((CmpF((unsigned long long)v123, 0.0) >> 5 & 3 | CmpF((unsigned long long)v123, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v123, 0.0) >> 5 & 3 | CmpF((unsigned long long)v123, 0.0) & 1) & (CmpF((unsigned long long)v123, 0.0) >> 5 & 3 | CmpF((unsigned long long)v123, 0.0) & 1) >> 1 & 1)) * 0x10000000, (v122 & 0xf0000000) >> 31 & 1))
    {
        v124 = 0;
    }
    else
    {
        v122 = v122 & 0xfffffff | ((((CmpF(v123, 1.0) >> 5 & 3 | CmpF(v123, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v123, 1.0) >> 5 & 3 | CmpF(v123, 1.0) & 1) & (CmpF(v123, 1.0) >> 5 & 3 | CmpF(v123, 1.0) & 1) >> 1 & 1)) * 0x10000000;
        v124 = (v122 & 0xf0000000 & 0x20000000 && !(v122 & 0xf0000000 & 0x40000000) ? 1.0 : v123);
    }
    if (v120 > 199)
    {
        v125 = g_20021e14;
    }
    else
    {
        v125 = 1;
        g_20021e14 = 1;
    }
    v126 = v18->field_c - v124;
    v127 = v122 & 0xfffffff | ((((CmpF(v126, 0.0) >> 5 & 3 | CmpF(v126, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v126, 0.0) >> 5 & 3 | CmpF(v126, 0.0) & 1) & (CmpF(v126, 0.0) >> 5 & 3 | CmpF(v126, 0.0) & 1) >> 1 & 1)) * 0x10000000;
    v129 = _INSERT(v128, 4, ((v127 & 0xf0000000) >> 31 & 1 ? v124 - v18->field_c : v126));
    v130 = (CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) >> 5 & 3 | CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) & 1) ^ 1;
    v131 = v127 & 0xfffffff | ((v130 * 0x40000000 - 1 >> 29) + 1 - ((CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) >> 5 & 3 | CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) & 1) & (CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) >> 5 & 3 | CmpF(*((unsigned int *)((void*)&v129 + 4)), 981668463) & 1) >> 1 & 1)) * 0x10000000;
    if ((v131 & 0xf0000000) >> 31 & 1 ^ v131 >> 28 & 1)
    {
        v132 = *(v77) - 1;
        if (480 >= v132)
        {
            *(v77) = 0;
            if (v125 == 1)
                goto LABEL_8028a6b;
            g_20022038 = 0;
LABEL_80278cd:
            if (32 < (v120 - g_200220b8 < 0 ? g_200220b8 - v120 : v120 - g_200220b8))
            {
                g_200220c4 = *((int *)(537010524 + v112 * 4)) - *((int *)(537010524 + (v112 - 1) * 4));
                v138 = g_200220c4;
                v131 = v131 & 0xfffffff | ((((CmpF(v138, 0x490ca000) >> 5 & 3 | CmpF(v138, 0x490ca000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(g_200220c4, 0x490ca000) >> 5 & 3 | CmpF(g_200220c4, 0x490ca000) & 1) & (CmpF(v138, 0x490ca000) >> 5 & 3 | CmpF(g_200220c4, 0x490ca000) & 1) >> 1 & 1)) * 0x10000000;
                v20 = &g_20021f90;
                g_200220b8 = v120;
                *((int *)&g_20021f90) = g_200220c4;
                v139 = g_200220c4;
                if (!((v131 & 0xf0000000) >> 30 & 1 | (v131 & 0xf0000000) >> 31 & 1 ^ (v131 & 0xf0000000) >> 28 & 1))
                {
                    do
                    {
                        v139 *= 0.5;
                        v131 = v131 & 0xfffffff | ((((CmpF(v139, 0x490ca000) >> 5 & 3 | CmpF(v139, 0x490ca000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v139, 0x490ca000) >> 5 & 3 | CmpF(v139, 0x490ca000) & 1) & (CmpF(v139, 0x490ca000) >> 5 & 3 | CmpF(v139, 0x490ca000) & 1) >> 1 & 1)) * 0x10000000;
                    } while (!((v131 & 0xf0000000) >> 30 & 1 | (v131 & 0xf0000000) >> 31 & 1 ^ (v131 & 0xf0000000) >> 28 & 1));
                }
                v140 = (unsigned int)(&g_8044d10.field_0)[1073 + -1 * (v120 >> 2)];
                v137 = (float)(v140 * v140 * v140 * v139);
                *(v20) = v137;
                if (g_20021cb0 == 1)
                {
                    v131 = v131 & 0xfffffff | ((((CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) & (CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v131 & 0xf0000000) >> 30 & 1 | (v131 & 0xf0000000) >> 31 & 1 ^ (v131 & 0xf0000000) >> 28 & 1))
                    {
                        v141 = (float)1059760799;
                        v142 = g_200220c4 * 1059760799;
                        v131 = v131 & 0xfffffff | ((((CmpF((unsigned int)v137, v142) >> 5 & 3 | CmpF((unsigned int)v137, v142) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, v142) >> 5 & 3 | CmpF((unsigned int)v137, v142) & 1) & (CmpF((unsigned int)v137, v142) >> 5 & 3 | CmpF((unsigned int)v137, v142) & 1) >> 1 & 1)) * 0x10000000;
                        v143 = (((v131 & 0xf0000000) >> 31 ^ 1) & 1 ? (int)g_200220c4 : v142);
                        if ((v131 & 0xf0000000) >> 31 & 1)
                        {
                            while (1)
                            {
                                v144 = v131 & 0xfffffff | ((((CmpF((unsigned int)v141, 0.75) >> 5 & 3 | CmpF((unsigned int)v141, 0.75) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v141, 0.75) >> 5 & 3 | CmpF((unsigned int)v141, 0.75) & 1) & (CmpF((unsigned int)v141, 0.75) >> 5 & 3 | CmpF((unsigned int)v141, 0.75) & 1) >> 1 & 1)) * 0x10000000;
                                v145 = v143 * 1059760799;
                                v146 = v143 * 0.75;
                                if ((v144 & 0xf0000000) >> 30 & 1)
                                {
                                    v131 = v144 & 0xfffffff | ((((CmpF((unsigned int)v137, v145) >> 5 & 3 | CmpF((unsigned int)v137, v145) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, v145) >> 5 & 3 | CmpF((unsigned int)v137, v145) & 1) & (CmpF((unsigned int)v137, v145) >> 5 & 3 | CmpF((unsigned int)v137, v145) & 1) >> 1 & 1)) * 0x10000000;
                                    if (!((v131 & 0xf0000000) >> 31 & 1))
                                        break;
                                    v143 = v145;
                                    v141 = 1059760799;
                                }
                                else
                                {
                                    v131 = v144 & 0xfffffff | ((((CmpF((unsigned int)v137, v146) >> 5 & 3 | CmpF((unsigned int)v137, v146) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, v146) >> 5 & 3 | CmpF((unsigned int)v137, v146) & 1) & (CmpF((unsigned int)v137, v146) >> 5 & 3 | CmpF((unsigned int)v137, v146) & 1) >> 1 & 1)) * 0x10000000;
                                    if (!((v131 & 0xf0000000) >> 31 & 1))
                                        break;
                                    v143 = v146;
                                    v141 = (float)0.75;
                                }
                            }
                        }
                        v137 = (float)v143;
                        *((unsigned int *)v20) = v143;
                    }
                }
            }
            else
            {
                v20 = &g_20021f90;
                if (!1)
                    goto LABEL_0x802ab23;
                v137 = g_20021f90;
                if (!1)
                    goto LABEL_802ab27;
                else
                    goto LABEL_802795f;
            }
        }
        else
        {
            *(v77) = v132;
            v18->field_c = v124;
            if (v125 != 1)
                goto LABEL_80287d1;
LABEL_802abeb:
            g_2002214c = 0;
            if (*((int *)0x200012a8) <= 1)
            {
                v147 = v124 * 0x43c80000;
                v148 = (399 <= v147 ? 399 : v147);
                *((float *)&g_200220dc) = (float)(int)(v124 * (64 + 64));
                v149 = (*((int *)0x200012a8) ? v148 & 0xfffffffc : v148 & 0xfffffffe);
                v32 = &g_200220e8;
                g_200220e8 = 0;
                if (v149 > 199)
                {
                    g_200220e8 = *((int *)((char *)&g_80462b0[v149] - 800));
                }
                else if (196 >= v149)
                {
                    *((unsigned int *)v32) = -(g_80462b0[196 + -1 * v149]);
                }
            }
            else
            {
                v129 = _INSERT(v129, 4, 1.0);
                v150 = v124 * 0x43a40000 - 16 & ~((int)(v124 * 0x43a40000 - 16) >> 31);
                g_200220e8 = g_8045e10[(295 <= v150 ? 295 : v150)];
                *((float *)&g_200220dc) = (float)(int)((v124 + *((unsigned int *)((void*)&v129 + 4))) * (32 + 32));
            }
LABEL_8028a6b:
            if (g_200220ec != 1)
            {
                v151 = *((int *)0x20021e6c);
                v152 = *((int *)0x20021e70);
                v92 = *((int *)0x20021338);
                v20 = &g_20021f90;
                v153 = g_20021e64 + *((int *)0x20021e68);
                v154 = *((int *)(537010524 + v112 * 4)) - *((int *)(537010524 + (v112 - 1) * 4));
            }
            else
            {
                v151 = *((int *)0x20021e6c);
                v20 = &g_20021f90;
                v152 = *((int *)0x20021e70);
                v92 = *((int *)0x20021338);
                v153 = g_20021e64 + *((int *)0x20021e68);
                v154 = g_20021fd8[g_20021cb8] - g_20021ff8[g_20021cb8];
            }
            v137 = (float)(int)v154;
            v121 = v153 + v151 + v152;
            g_20021f90 = v137;
        }
    }
    else
    {
        v18->field_c = v124;
        *(v77) = 960;
        if (v125 == 1)
            goto LABEL_802abeb;
LABEL_80287d1:
        g_20022038 = 0;
        if (*((int *)0x200012a8) > 1)
        {
            v133 = v124 * 0x43a40000 - 16 & ~((int)(v124 * 0x43a40000 - 16) >> 31);
            g_200220e8 = g_8045e10[(295 <= v133 ? 295 : v133)];
            *((float *)&g_200220dc) = (float)(int)((v124 + 1.0) * (32 + 32));
            goto LABEL_80278cd;
        }
        else
        {
            if (!1)
                goto LABEL_0x802aa9b;
            v134 = v124 * 0x43c80000;
            v135 = (399 <= v134 ? 399 : v134);
            *((float *)&g_200220dc) = (float)(int)(v124 * (64 + 64));
            v136 = (*((int *)0x200012a8) ? v135 & 0xfffffffc : v135 & 0xfffffffe);
            v32 = &g_200220e8;
            g_200220e8 = 0;
            if (v136 > 199)
            {
                g_200220e8 = *((int *)((char *)&g_80462b0[v136] - 800));
                goto LABEL_80278cd;
            }
            else if (196 >= v136)
            {
                *((unsigned int *)v32) = -(g_80462b0[196 + -1 * v136]);
                goto LABEL_80278cd;
            }
        }
    }
LABEL_802795f:
    if (v92 == 1)
        g_200220e8 = 0x3f800000;
    v0 = g_20021c6c;
    v155 = g_20021c6c / (0x800 + 0x800);
    v156 = g_20021c78 * 965355262;
    v157 = v155 * 1107817923;
    v158 = (21 <= v157 ? 21 : v157);
    v159 = v158 * 4;
    v160 = *((int *)(536871904 + v159));
    v161 = *((int *)(536871768 + v159));
    node = g_20021c78;
    v162 = v131 & 0xfffffff | ((((CmpF(v156, 1028131956) >> 5 & 3 | CmpF(v156, 1028131956) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v156, 1028131956) >> 5 & 3 | CmpF(v156, 1028131956) & 1) & (CmpF(v156, 1028131956) >> 5 & 3 | CmpF(v156, 1028131956) & 1) >> 1 & 1)) * 0x10000000;
    v50 = *((int *)(536872176 + v158 * 4));
    g_20021f48 = v160;
    g_2002209c = v161;
    if ((v162 & 0xf0000000) >= 0)
    {
        v162 = v162 & 0xfffffff | ((((CmpF(v156, 1065762916) >> 5 & 3 | CmpF(v156, 1065762916) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v156, 1065762916) >> 5 & 3 | CmpF(v156, 1065762916) & 1) & (CmpF(v156, 1065762916) >> 5 & 3 | CmpF(v156, 1065762916) & 1) >> 1 & 1)) * 0x10000000;
        v163 = (((v162 & 0xf0000000) >> 3 ^ v162 & 0xf0000000) & 0x10000000 ? v156 - 1028131956 : v156);
        if (!((v162 & 0xf0000000) >> 31 & 1 ^ v162 >> 28 & 1))
            v163 = 0x3f7fffef;
    }
    else
    {
        if (!1)
            goto LABEL_0x8028877;
        v163 = 0;
        if (!1)
            goto LABEL_802887b;
        else
            goto LABEL_8027a91;
    }
LABEL_8027a91:
    if (!*((int *)0x2002208c))
    {
        v18->field_10 = v163;
        v164 = v163;
LABEL_8027aa5:
        if (g_20022140 > 0)
        {
            g_20022140 = g_20022140 - 1;
            if (!g_20022140)
                v18->field_10 = v163;
            else
                v163 = v164;
            if (g_20021e14 == 1)
                goto LABEL_802878d;
            goto LABEL_8027ac9;
        }
        else
        {
            v163 = v164;
        }
    }
    else
    {
        if (!1)
            goto LABEL_0x802873d;
        v164 = v18->field_10;
        if (!1)
            goto LABEL_0x8028741;
        v165 = v163 - v164;
        v166 = v162 & 0xfffffff | ((((CmpF(v165, 0.0) >> 5 & 3 | CmpF(v165, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v165, 0.0) >> 5 & 3 | CmpF(v165, 0.0) & 1) & (CmpF(v165, 0.0) >> 5 & 3 | CmpF(v165, 0.0) & 1) >> 1 & 1)) * 0x10000000;
        v167 = ((v166 & 0xf0000000) >> 31 & 1 ? v164 - v163 : v165);
        v162 = v166 & 0xfffffff | ((((CmpF(v167, 1006834287) >> 5 & 3 | CmpF(v167, 1006834287) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v167, 1006834287) >> 5 & 3 | CmpF(v167, 1006834287) & 1) & (CmpF(v167, 1006834287) >> 5 & 3 | CmpF(v167, 1006834287) & 1) >> 1 & 1)) * 0x10000000;
        if ((v162 & 0xf0000000) >> 30 & 1 | (v162 & 0xf0000000) >> 31 & 1 ^ (v162 & 0xf0000000) >> 28 & 1)
            goto LABEL_8027aa5;
        v18->field_10 = v163;
        if (v24 != 1 && *((int *)0x200012b0) != 2)
        {
            g_20022140 = 0;
        }
        else
        {
            g_20022140 = 149;
            if (g_20021e14 == 1)
            {
LABEL_802878d:
                v162 = v162 & 0xfffffff | ((((CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) & (CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) >> 1 & 1)) * 0x10000000;
                if (!((v162 & 0xf0000000) >> 30 & 1 | (v162 & 0xf0000000) >> 31 & 1 ^ (v162 & 0xf0000000) >> 28 & 1))
                {
                    v129 = _INSERT(v129, 4, 0.5);
                    do
                    {
                        v137 = (float)(v137 * *((unsigned int *)((void*)&v129 + 4)));
                        v162 = v162 & 0xfffffff | ((((CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) & (CmpF((unsigned int)v137, 0x459c4000) >> 5 & 3 | CmpF((unsigned int)v137, 0x459c4000) & 1) >> 1 & 1)) * 0x10000000;
                    } while (!((v162 & 0xf0000000) >> 30 & 1 | (v162 & 0xf0000000) >> 31 & 1 ^ (v162 & 0xf0000000) >> 28 & 1));
                    v155 = 1035256401;
                    *(v20) = v137;
                }
                else
                {
                    v155 = 1035256401;
                }
            }
LABEL_8027ac9:
            g_2002214c = 0;
            g_20021e14 = 0;
        }
    }
    v168 = *((int *)0x20021e74) + *((int *)0x20021e78) + v121;
    v169 = (v168 - 1.0) * 6.0 + 3;
    v170 = (33 <= v169 ? 33 : v169);
    v171 = *((int *)(536872040 + (v170 & ~(v170 >> 31)) * 4));
    if (v92 != 1)
    {
LABEL_8027b1f:
        v172 = v162 & 0xfffffff | ((((CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) & (CmpF((unsigned int)v137, 8.0) >> 5 & 3 | CmpF((unsigned int)v137, 8.0) & 1) >> 1 & 1)) * 0x10000000;
        if ((v172 & 0xf0000000) >> 31 & 1)
        {
            v173 = v172 & 0xfffffff | ((((CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) & (CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) >> 1 & 1)) * 0x10000000;
            if (!((v173 & 0xf0000000) >> 31 & 1 ^ v173 >> 28 & 1))
            {
                v174 = v173 & 0xfffffff | ((((CmpF((unsigned int)g_20022094, 8.0) >> 5 & 3 | CmpF((unsigned int)g_20022094, 8.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)g_20022094, 8.0) >> 5 & 3 | CmpF((unsigned int)g_20022094, 8.0) & 1) & (CmpF((unsigned int)g_20022094, 8.0) >> 5 & 3 | CmpF((unsigned int)g_20022094, 8.0) & 1) >> 1 & 1)) * 0x10000000;
                *((unsigned int *)v20) = 8.0;
                if ((v174 & 0xf0000000) >> 30 & 1)
                {
                    v63 = &g_20021e0c;
                    v175 = 4.0;
                    g_20021e0c = 4.0;
                    goto LABEL_8027b59;
                }
                else
                {
                    v137 = (float)8.0;
                    v175 = 4.0;
                }
LABEL_8028925:
                v174 = v174 & 0xfffffff | ((((CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) & (CmpF((unsigned int)v137, 0.0) >> 5 & 3 | CmpF((unsigned int)v137, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                g_20022094 = v137;
                g_20021e84 = 1.0 / v137;
                if (!(v174 & 0xf0000000 & 0x20000000) || v174 & 0xf0000000 & 0x40000000)
                    goto LABEL_802ab41;
            }
            else
            {
LABEL_802ab27:
                v176 = ((CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) ^ 1) * 0x40000000;
                v174 = v173 & 0xfffffff | ((v176 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) & (CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) >> 1 & 1)) * 0x10000000;
                v175 = v137 * 0.5;
                if (!((v174 & 0xf0000000) >> 30 & 1))
                {
                    g_20022094 = v137;
LABEL_802ab41:
                    g_20021e84 = 0;
                }
            }
        }
        else
        {
            v174 = v172 & 0xfffffff | ((((CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) & (CmpF((unsigned int)v137, (unsigned int)g_20022094) >> 5 & 3 | CmpF((unsigned int)v137, (unsigned int)g_20022094) & 1) >> 1 & 1)) * 0x10000000;
            v175 = v137 * 0.5;
            if (!((v174 & 0xf0000000) >> 30 & 1))
                goto LABEL_8028925;
        }
    }
    else
    {
        if (!1)
            goto LABEL_0x8028901;
        v162 = v162 & 0xfffffff | ((((CmpF((unsigned int)v137, 0x48bb8000) >> 5 & 3 | CmpF((unsigned int)v137, 0x48bb8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v137, 0x48bb8000) >> 5 & 3 | CmpF((unsigned int)v137, 0x48bb8000) & 1) & (CmpF((unsigned int)v137, 0x48bb8000) >> 5 & 3 | CmpF((unsigned int)v137, 0x48bb8000) & 1) >> 1 & 1)) * 0x10000000;
        if ((v162 & 0xf0000000) >> 30 & 1 | (v162 & 0xf0000000) >> 31 & 1 ^ (v162 & 0xf0000000) >> 28 & 1)
            goto LABEL_8027b1f;
        v137 = 0x48bb8000;
        v175 = 0x483b8000;
        *(v20) = 0x48bb8000;
        v174 = v162 & 0xfffffff | ((((CmpF(0x48bb8000, (unsigned int)g_20022094) >> 5 & 3 | CmpF(0x48bb8000, (unsigned int)g_20022094) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(0x48bb8000, (unsigned int)g_20022094) >> 5 & 3 | CmpF(0x48bb8000, (unsigned int)g_20022094) & 1) & (CmpF(0x48bb8000, (unsigned int)g_20022094) >> 5 & 3 | CmpF(0x48bb8000, (unsigned int)g_20022094) & 1) >> 1 & 1)) * 0x10000000;
        if (!((v174 & 0xf0000000) >> 30 & 1))
            goto LABEL_8028925;
    }
    v63 = &g_20021e0c;
    v174 = v174 & 0xfffffff | ((((CmpF(v175, 0x46bb8000) >> 5 & 3 | CmpF(v175, 0x46bb8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v175, 0x46bb8000) >> 5 & 3 | CmpF(v175, 0x46bb8000) & 1) & (CmpF(v175, 0x46bb8000) >> 5 & 3 | CmpF(v175, 0x46bb8000) & 1) >> 1 & 1)) * 0x10000000;
    g_20021e0c = v175;
    if ((v174 & 0xf0000000) >> 30 & 1 | (v174 & 0xf0000000) >> 31 & 1 ^ (v174 & 0xf0000000) >> 28 & 1)
    {
LABEL_8027b59:
        v64 = &g_20021e08;
        v174 = v174 & 0xfffffff | ((((CmpF(v175, 0x437a0000) >> 5 & 3 | CmpF(v175, 0x437a0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v175, 0x437a0000) >> 5 & 3 | CmpF(v175, 0x437a0000) & 1) & (CmpF(v175, 0x437a0000) >> 5 & 3 | CmpF(v175, 0x437a0000) & 1) >> 1 & 1)) * 0x10000000;
        g_20021e08 = g_20021e84 + g_20021e84;
        if (!((v174 & 0xf0000000) >> 30 & 1 | (v174 & 0xf0000000) >> 31 & 1 ^ (v174 & 0xf0000000) >> 28 & 1))
            goto LABEL_8028967;
        v177 = v174 & 0xfffffff | ((((CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) & (CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) >> 1 & 1)) * 0x10000000;
        if (!((v177 & 0xf0000000) >> 30 & 1))
            goto LABEL_8027b95;
        goto LABEL_8027b89;
    }
    else
    {
        v64 = &g_20021e08;
        g_20021e0c = 0x46bb8000;
        g_20021e08 = 942588734;
LABEL_8028967:
        v177 = v174 & 0xfffffff | ((((CmpF(g_2002209c, 1.0) >> 5 & 3 | CmpF(g_2002209c, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(g_2002209c, 1.0) >> 5 & 3 | CmpF(g_2002209c, 1.0) & 1) & (CmpF(g_2002209c, 1.0) >> 5 & 3 | CmpF(g_2002209c, 1.0) & 1) >> 1 & 1)) * 0x10000000;
        if ((v177 & 0xf0000000) < 0 || !*((int *)0x200012d0))
            goto LABEL_8027b89;
        v177 = v177 & 0xfffffff | ((((CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) & (CmpF(v160, 1.0) >> 5 & 3 | CmpF(v160, 1.0) & 1) >> 1 & 1)) * 0x10000000;
        if ((v177 & 0xf0000000) >> 30 & 1)
        {
LABEL_8027b89:
            *(v63) = 0x437a0000;
            *(v64) = 998445679;
            goto LABEL_8027b95;
        }
    }
LABEL_8027b95:
    if (g_200012e0 != 1)
    {
        if (g_200012e0 == 0xffffffff && !v92)
        {
            g_200012e0 = 0;
            v178 = g_20021cbc - 1;
            g_20021cbc = (v178 <= 0 ? *((int *)0x2002208c) : v178);
        }
LABEL_8027ba5:
        if (g_20021320 == 1)
            goto LABEL_802a969;
    }
    else
    {
        if (v92)
            goto LABEL_8027ba5;
        g_200012e0 = 0;
        v179 = g_20021cbc + 1;
        g_20021cbc = (*((int *)537010316) < v179 ? 1 : v179);
        if (g_20021320 == 1)
        {
LABEL_802a969:
            if (v76 || !g_20021cb0)
            {
                v43 = &g_20022108;
                if (!g_20022108)
                {
                    v40 = &g_200213a8;
                    g_20022074 = g_20022074 | 8;
                    g_20022108 = 1;
                    if (*((int *)0x200012c4))
                    {
                        if (!g_200213a8)
                            goto LABEL_802ad2b;
LABEL_802a9a7:
                        v44 = &g_20022114;
                        g_20022114 = (&g_20021fb8)[g_20021cb8];
                        v180 = *(0x20021ca4);
                        if (!*((int *)0x2002208c))
                        {
                            v181 = *((int *)537010524);
                            g_2002210c = *((int *)0x2002215c);
                            *(v44) = 1;
                            if (*(0x2002215c) >= *((int *)537009316))
                                goto LABEL_802a9d3;
                        }
                        else
                        {
                            if (!g_20021de4 || g_20021e14 != 1 || (v177 = v177 & 0xfffffff | ((((CmpF((unsigned long long)v160, (unsigned long long)1.0) >> 5 & 3 | CmpF((unsigned long long)v160, (unsigned long long)1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v160, (unsigned long long)1.0) >> 5 & 3 | CmpF((unsigned long long)v160, (unsigned long long)1.0) & 1) & (CmpF((unsigned long long)v160, (unsigned long long)1.0) >> 5 & 3 | CmpF((unsigned long long)v160, (unsigned long long)1.0) & 1) >> 1 & 1)) * 0x10000000, !((v177 & 0xf0000000) >> 30 & 1)))
                            {
                                v181 = *((int *)(537010524 + (g_20021de4 - 1) * 4));
                                g_2002210c = v181;
                            }
                            else
                            {
                                v182 = *((int *)(537010524 + g_20021de4 * 4));
                                v183 = *((int *)(v12 + g_20021de4 * 4));
                                v129 = _INSERT(v129, 4, v183 * v163);
                                g_2002210c = (int)*((unsigned int *)((void*)&v129 + 4)) + *((int *)(537009960 + g_20021cb8 * 4)) & *((int *)0x20021ca0);
                                if (g_2002210c > v182)
                                    *((float *)&g_2002210c) = (float)(int)(g_2002210c - v183);
                                v181 = g_2002210c;
                                if (2000 > v181 - *((int *)(537010524 + (g_20021de4 - 1) * 4)))
                                {
                                    g_2002210c = *((int *)(537010524 + (g_20021de4 - 1) * 4));
                                    v181 = g_2002210c;
                                }
                            }
LABEL_802ad41:
                            if (v180 <= v181 || 300 < *(v44))
                            {
LABEL_802a9d3:
                                g_200213b8 = 2000;
                                g_2002133c = 2000;
                                g_20021c9c = 0;
                                *((unsigned int *)v43) = 0;
                                g_20021320 = 0;
                            }
                        }
                    }
                    else
                    {
                        if (!g_200213a8)
                            goto LABEL_802a9a7;
LABEL_802ad2b:
                        v181 = *((int *)(537010524 + *((int *)0x2002208c) * 4));
                        v180 = *(0x20021ca4);
                        v44 = &g_20022114;
                        v184 = *((int *)0x2002208c) + 1;
                        g_2002210c = v181;
                        g_20022114 = v184;
                        goto LABEL_802ad41;
                    }
                    g_2002207c = 869711765;
                    g_2002131c = v181 * 8;
                    g_20021318 = 0;
                    *((unsigned int *)v40) = 0;
                }
            }
        }
    }
    if (0 >= g_20024124)
        goto LABEL_0x8027bbf;
    if ((Load(addr=537018660<32>, size=4, endness=Iend_LE) CmpGT 0<32>)) { Goto None } else { Goto None }
    v185 = g_20024124 * 1008981770;
    v186 = v177 & 0xfffffff | ((((CmpF(v185, 1.0) >> 5 & 3 | CmpF(v185, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v185, 1.0) >> 5 & 3 | CmpF(v185, 1.0) & 1) & (CmpF(v185, 1.0) >> 5 & 3 | CmpF(v185, 1.0) & 1) >> 1 & 1)) * 0x10000000;
    if (!((v186 & 0xf0000000) >> 30 & 1 | (v186 & 0xf0000000) >> 31 & 1 ^ (v186 & 0xf0000000) >> 28 & 1) || *((int *)0x200012a8))
    {
        v48 = (float)1.0;
    }
    else
    {
        v186 = v186 & 0xfffffff | ((((CmpF(v185, 1025708176) >> 5 & 3 | CmpF(v185, 1025708176) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v185, 1025708176) >> 5 & 3 | CmpF(v185, 1025708176) & 1) & (CmpF(v185, 1025708176) >> 5 & 3 | CmpF(v185, 1025708176) & 1) >> 1 & 1)) * 0x10000000;
        v187 = (((v186 & 0xf0000000) >> 3 ^ v186 & 0xf0000000) & 0x10000000 ? 1025708176 : v185);
        v48 = (float)(v187 * v187 * v187);
    }
    v188 = g_20022154;
    v189 = g_20022154 - v163;
    v190 = v186 & 0xfffffff | ((((CmpF(v189, 1017370378) >> 5 & 3 | CmpF(v189, 1017370378) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v189, 1017370378) >> 5 & 3 | CmpF(v189, 1017370378) & 1) & (CmpF(v189, 1017370378) >> 5 & 3 | CmpF(v189, 1017370378) & 1) >> 1 & 1)) * 0x10000000;
    if ((v190 & 0xf0000000) >> 30 & 1 | (v190 & 0xf0000000) >> 31 & 1 ^ (v190 & 0xf0000000) >> 28 & 1)
    {
        v190 = v190 & 0xfffffff | ((((CmpF(v189, 3164854026) >> 5 & 3 | CmpF(v189, 3164854026) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v189, 3164854026) >> 5 & 3 | CmpF(v189, 3164854026) & 1) & (CmpF(v189, 3164854026) >> 5 & 3 | CmpF(v189, 3164854026) & 1) >> 1 & 1)) * 0x10000000;
        v52 = (float)(((v190 & 0xf0000000) >> 31 ^ 1) & 1 ? 953267991 : 1.0);
    }
    else
    {
        if (!1)
            goto LABEL_0x802a7d1;
        v52 = (float)1.0;
        if (!1)
            goto LABEL_802a7d5;
        else
            goto LABEL_8027c17;
    }
LABEL_8027c17:
    v191 = v190 & 0xfffffff | ((((CmpF(v168, 2.0) >> 5 & 3 | CmpF(v168, 2.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v168, 2.0) >> 5 & 3 | CmpF(v168, 2.0) & 1) & (CmpF(v168, 2.0) >> 5 & 3 | CmpF(v168, 2.0) & 1) >> 1 & 1)) * 0x10000000;
    v47 = a2;
    v192 = a2;
    if ((v191 & 0xf0000000) >> 30 & 1 | (v191 & 0xf0000000) >> 31 & 1 ^ (v191 & 0xf0000000) >> 28 & 1)
    {
        if (v192 > 0)
        {
            v38 = &g_20022038;
            v35 = &g_20021cb8;
            v32 = &g_200220e8;
            v30 = &g_200220ec;
            v16 = 0x200012a8;
            v43 = &g_20022108;
            v40 = &g_200213a8;
            v44 = &g_20022114;
            v58 = &g_200220e0;
            v21 = &g_20022084;
            v55 = &g_200220d4;
            v17 = &g_200220f8;
            v56 = 0x20022124;
            v53 = &g_20001054;
            v36 = &g_20021e18;
            v31 = &g_20021e20;
            v6 = &g_20021f4c[0];
            v62 = 537010312;
            v59 = &g_20022158;
            v74 = 1008981770;
            v193 = 0;
            v54 = &g_20022064;
            v33 = &g_20022090;
            v45 = &g_200213c0;
            v19 = &g_200220c8;
            v60 = &g_20021dc0;
            v57 = &g_20021cc0;
            v61 = &g_20021c88;
            v73 = v171 * 1008981770;
            v37 = 0;
            v194 = v163;
            while (1)
            {
                v195 = a0;
                v196 = *(v55);
                v197 = v18;
                v198 = v197->field_0;
                v199 = *(v58);
                v200 = *(v32);
                v66 = v193 * 2;
                v67 = v66 + 2;
                v201 = *((short *)(v195 + v193 * 2)) * 0x38000100;
                v202 = UnaryOp Abs;
                v203 = v201 * 1.5;
                v204 = v191 & 0xfffffff | ((((CmpF(v202, v196) >> 5 & 3 | CmpF(v202, v196) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v202, v196) >> 5 & 3 | CmpF(v202, v196) & 1) & (CmpF(v202, v196) >> 5 & 3 | CmpF(v202, v196) & 1) >> 1 & 1)) * 0x10000000;
                v205 = *((short *)(v195 + v67)) * 0x38000100;
                v206 = v204 & 0xfffffff | ((((CmpF(v203, 1.0) >> 5 & 3 | CmpF(v203, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v203, 1.0) >> 5 & 3 | CmpF(v203, 1.0) & 1) & (CmpF(v203, 1.0) >> 5 & 3 | CmpF(v203, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                v207 = (v206 & 0xf0000000 & 0x20000000 && !(v206 & 0xf0000000 & 0x40000000) ? 1.0 : v203);
                v208 = v205 * 1.5;
                v209 = v206 & 0xfffffff | ((((CmpF(v207, -1.0) >> 5 & 3 | CmpF(v207, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v207, -1.0) >> 5 & 3 | CmpF(v207, -1.0) & 1) & (CmpF(v207, -1.0) >> 5 & 3 | CmpF(v207, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                v210 = v209 & 0xfffffff | ((((CmpF(v208, 1.0) >> 5 & 3 | CmpF(v208, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v208, 1.0) >> 5 & 3 | CmpF(v208, 1.0) & 1) & (CmpF(v208, 1.0) >> 5 & 3 | CmpF(v208, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                v211 = *(v21) + (v155 - *(v21)) * v74;
                v212 = (((v209 & 0xf0000000) >> 3 ^ v209 & 0xf0000000) & 0x10000000 ? -1.0 : v207);
                v213 = (v210 & 0xf0000000 & 0x20000000 && !(v210 & 0xf0000000 & 0x40000000) ? 1.0 : v208);
                v214 = 907633515 + 196314165 * *(v17);
                *(v17) = v214;
                v215 = v199 + (v200 - v199) * v48;
                v216 = (((v204 & 0xf0000000) >> 3 ^ v204 & 0xf0000000) & 0x10000000 ? v196 : v202) * 0x3f7fffac;
                v217 = v210 & 0xfffffff | ((((CmpF(v213, -1.0) >> 5 & 3 | CmpF(v213, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v213, -1.0) >> 5 & 3 | CmpF(v213, -1.0) & 1) & (CmpF(v213, -1.0) >> 5 & 3 | CmpF(v213, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                *(v55) = v216;
                v218 = v217 & 0xfffffff | ((((CmpF(v211, 0.25) >> 5 & 3 | CmpF(v211, 0.25) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v211, 0.25) >> 5 & 3 | CmpF(v211, 0.25) & 1) & (CmpF(v211, 0.25) >> 5 & 3 | CmpF(v211, 0.25) & 1) >> 1 & 1)) * 0x10000000;
                g_20022154 = v188 + (v194 - v188) * v52;
                v219 = (((v217 & 0xf0000000) >> 3 ^ v217 & 0xf0000000) & 0x10000000 ? -1.0 : v213);
                v197->field_0 = v198 + (v49 - v198) * 981668463;
                *(v58) = v215;
                *(v21) = v211;
                if (v218 & 0x20000000 && !(v218 & 0x40000000) && !(v0 = g_20021c7c, v220 = (unsigned int)(int)(long long)(int)v0, v218 = v218 & 0xfffffff | ((((CmpF((unsigned long long)v220, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)v220, 0x44fa0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) & 1) & (CmpF((unsigned long long)v220, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) & 1) >> 1 & 1)) * 0x10000000, (v218 & 0xf0000000) < 0))
                    v221 = (-0.5 + v214 * *((int *)0x20022098)) * 0x461c4000 * v211 * v211;
                else
                    v221 = 0;
                if (v16[8])
                {
                    v218 = v218 & 0xfffffff | ((((CmpF(v216, 981668463) >> 5 & 3 | CmpF(v216, 981668463) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v216, 981668463) >> 5 & 3 | CmpF(v216, 981668463) & 1) & (CmpF(v216, 981668463) >> 5 & 3 | CmpF(v216, 981668463) & 1) >> 1 & 1)) * 0x10000000;
                    v215 = ((v218 & 0xf0000000) >> 31 & 1 ? v215 + v205 * ((v218 & 0xf0000000) >> 31 & 1 ? 3.0 : 981668463) : v215);
                }
                v222 = (unsigned int)(float)(int)v215;
                v223 = v215 - (int)v222;
                if (g_20021e14 != 1)
                {
                    v224 = 0;
                    v25 = 1;
                }
                else
                {
                    v218 = v218 & 0xfffffff | ((((CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) & (CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v218 & 0xf0000000) >> 30 & 1 | (v218 & 0xf0000000) >> 31 & 1 ^ (v218 & 0xf0000000) >> 28 & 1))
                    {
                        v224 = v223;
                        v25 = v222;
                    }
                    else
                    {
                        v225 = -(v215);
                        v25 = (unsigned int)(float)(int)v225;
                        v224 = v225 - (int)v25;
                    }
                }
                idx = v56;
                v78 = v222;
                v82 = v223;
                v227 = v215 * *(idx);
                v228 = v215 * idx[1];
                v229 = v215 * idx[2];
                v230 = (unsigned int)(float)(int)v227;
                v231 = (unsigned int)(float)(int)v228;
                v232 = (unsigned int)(float)(int)v229;
                v233 = (int)v230;
                v234 = v227 - v233;
                v235 = v228 - (int)v231;
                v236 = v229 - (int)v232;
                v237 = v218 & 0xfffffff | ((((CmpF(v223, 0.0) >> 5 & 3 | CmpF(v223, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v223, 0.0) >> 5 & 3 | CmpF(v223, 0.0) & 1) & (CmpF(v223, 0.0) >> 5 & 3 | CmpF(v223, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                v79 = v230;
                v83 = v234;
                v80 = v231;
                v84 = v235;
                v81 = v232;
                v85 = v236;
                if ((v237 & 0xf0000000) >> 31 & 1)
                {
                    v82 = v223 + 1.0;
                    v78 = (int)v222 - 1;
                }
                v238 = v237 & 0xfffffff | ((((CmpF(v234, 0.0) >> 5 & 3 | CmpF(v234, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v234, 0.0) >> 5 & 3 | CmpF(v234, 0.0) & 1) & (CmpF(v234, 0.0) >> 5 & 3 | CmpF(v234, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v238 & 0xf0000000) >> 31 & 1)
                {
                    v83 = v234 + 1.0;
                    v79 = (int)v230 - 1;
                }
                v239 = v238 & 0xfffffff | ((((CmpF(v235, 0.0) >> 5 & 3 | CmpF(v235, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v235, 0.0) >> 5 & 3 | CmpF(v235, 0.0) & 1) & (CmpF(v235, 0.0) >> 5 & 3 | CmpF(v235, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v239 & 0xf0000000) >> 31 & 1)
                {
                    v84 = v235 + 1.0;
                    v80 = (int)v231 - 1;
                }
                v240 = v239 & 0xfffffff | ((((CmpF(v236, 0.0) >> 5 & 3 | CmpF(v236, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v236, 0.0) >> 5 & 3 | CmpF(v236, 0.0) & 1) & (CmpF(v236, 0.0) >> 5 & 3 | CmpF(v236, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v240 & 0xf0000000) >> 31 & 1)
                {
                    v85 = v236 + 1.0;
                    v81 = (int)v232 - 1;
                }
                if (g_20021cb0 != 1)
                {
LABEL_802801b:
                    v241 = (unsigned int)*(v20);
                    if (g_20021ca8 != 1 || v37)
                        goto LABEL_802802b;
                    v242 = (int)g_2002214c;
                    v243 = (g_20021c74 < 2000 ? v242 - v241 : v242);
                    v244 = *((int *)(v12 + g_20021de4 * 4));
                    *((float *)&g_2002214c) = (float)(int)(2000 <= g_20021c74 ? v243 + v241 : v243);
                    v245 = (int)g_2002214c;
                    v240 = v240 & 0xfffffff | ((((CmpF(v245, v244) >> 5 & 3 | CmpF(v245, v244) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v245, v244) >> 5 & 3 | CmpF(v245, v244) & 1) & (CmpF(v245, v244) >> 5 & 3 | CmpF(v245, v244) & 1) >> 1 & 1)) * 0x10000000;
                    if ((v240 & 0xf0000000) >> 30 & 1 | (v240 & 0xf0000000) >> 31 & 1 ^ (v240 & 0xf0000000) >> 28 & 1)
                    {
                        v240 = v240 & 0xfffffff | ((((CmpF(v245, 0.0) >> 5 & 3 | CmpF(v245, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v245, 0.0) >> 5 & 3 | CmpF(v245, 0.0) & 1) & (CmpF(v245, 0.0) >> 5 & 3 | CmpF(v245, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                        if ((v240 & 0xf0000000) >> 31 & 1)
                            *((float *)&g_2002214c) = (float)(int)(v244 - 1.0);
                    }
                    else
                    {
                        g_2002214c = 0;
                    }
                    v246 = *(v53);
                    if (g_20021e14 != 1)
                    {
                        *(v36) = *(v31);
LABEL_8029307:
                        if (v246 == 1)
                        {
                            v248 = *(v36);
                            goto LABEL_8029321;
                        }
                    }
                    else if (v246 == 1)
                    {
                        if (!1)
                            goto LABEL_802a811;
                        else
                            goto LABEL_802a811;
                        v247 = *(v36);
                        v248 = v247;
LABEL_802a811:
                        v240 = v240 & 0xfffffff | ((((CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) & (CmpF(v215, 0.0) >> 5 & 3 | CmpF(v215, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                        if ((v240 & 0xf0000000) < 0)
                        {
                            if (!1)
                                goto LABEL_0x802ab5b;
                            *(v36) = v248 - v215;
                            if (!1)
                                goto LABEL_802ab5f;
                            else
                                goto LABEL_8028035;
                        }
                        else if (v240 & 0x40000000 || (v240 & 0xf0000000) >> 3 >> 28 & 1 ^ v240 >> 28 & 1)
                        {
LABEL_8029321:
                            *(v36) = v248 + 1.0;
                            goto LABEL_8028035;
                        }
                        else
                        {
                            *(v36) = v215 + v248;
                            goto LABEL_8028035;
                        }
                    }
                }
                else
                {
                    v240 = v240 & 0xfffffff | ((((CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) & (CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) >> 1 & 1)) * 0x10000000;
                    if ((v240 & 0xf0000000) >> 31 & 1)
                    {
                        if (v16[6] != 1)
                            goto LABEL_802942d;
                        goto LABEL_802801b;
                    }
                    if (v16[6] != 2)
                        goto LABEL_802801b;
LABEL_802942d:
                    v241 = (unsigned int)*(v20);
                    if (!g_20021e14)
                    {
                        v249 = (g_20021c74 < 2000 ? g_20022148 - g_20022144 : g_20022148);
                        v233 = (2000 <= g_20021c74 ? v249 + g_20022144 : v249);
                        v250 = (unsigned int)(float)(int)v233;
                        v251 = (int)v250;
                        v252 = g_2002214c + v251;
                        g_20022148 = v233 - (int)v250;
                        g_2002214c = v252;
                        if (v251 < 0)
                        {
                            v252 -= 1;
                            g_20022148 = g_20022148 + 1.0;
                            g_2002214c = v252;
                        }
                        v253 = *((int *)(v12 + g_20021de4 * 4));
                        v246 = *(v53);
                        v254 = v252;
                        v240 = v240 & 0xfffffff | ((((CmpF(v254, v253) >> 5 & 3 | CmpF(v254, v253) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v252, v253) >> 5 & 3 | CmpF(v252, v253) & 1) & (CmpF(v252, v253) >> 5 & 3 | CmpF(v252, v253) & 1) >> 1 & 1)) * 0x10000000;
                        if (!((v240 & 0xf0000000) >> 30 & 1 | (v240 & 0xf0000000) >> 31 & 1 ^ (v240 & 0xf0000000) >> 28 & 1))
                        {
                            g_2002214c = 0;
                            goto LABEL_8029307;
                        }
                        else if (0 > v252)
                        {
                            *((float *)&g_2002214c) = (float)(int)(v253 - 1.0);
                            goto LABEL_8029307;
                        }
                    }
LABEL_802802b:
                    if (*(v53) == 1)
                    {
                        v248 = *(v36);
                        if (g_20021e14 == 1)
                            goto LABEL_802a811;
                        goto LABEL_8029321;
                    }
LABEL_8028035:
                    idx1 = *(v35);
                    v256 = *((int *)(v6 + idx1 * 4));
                    v257 = g_20021f48 * v241;
                    v258 = v256 * g_20021f48;
                    v259 = v240 & 0xfffffff | ((((CmpF(v258, v257) >> 5 & 3 | CmpF(v258, v257) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v258, v257) >> 5 & 3 | CmpF(v258, v257) & 1) & (CmpF(v258, v257) >> 5 & 3 | CmpF(v258, v257) & 1) >> 1 & 1)) * 0x10000000;
                    v260 = (((v259 & 0xf0000000) >> 30 | (v259 & 0xf0000000) >> 31 ^ (v259 & 0xf0000000) >> 28) & 1 ? v258 : v257);
                    v261 = v259 & 0xfffffff | ((((CmpF(v260, 1.0) >> 5 & 3 | CmpF(v260, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v260, 1.0) >> 5 & 3 | CmpF(v260, 1.0) & 1) & (CmpF(v260, 1.0) >> 5 & 3 | CmpF(v260, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v259 & 0xf0000000) >> 30 & 1 | (v259 & 0xf0000000) >> 31 & 1 ^ (v259 & 0xf0000000) >> 28 & 1))
                        goto LABEL_0x8028069;
                    *(v31) = v258;
                    if ((v259 & 0xf0000000) >> 30 & 1 | (v259 & 0xf0000000) >> 31 & 1 ^ (v259 & 0xf0000000) >> 28 & 1)
                        goto LABEL_802806d;
                    else
                        goto LABEL_802806d;
                    *(v31) = v260;
LABEL_802806d:
                    v262 = (((v261 & 0xf0000000) >> 30 | (v261 & 0xf0000000) >> 31 ^ (v261 & 0xf0000000) >> 28) & 1 ^ 1 ? v260 : (((v261 & 0xf0000000) >> 30 | (v261 & 0xf0000000) >> 31 ^ (v261 & 0xf0000000) >> 28) & 1 ? 1.0 : v233));
                    v263 = v262 - v256;
                    v264 = v261 & 0xfffffff | ((((CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) & (CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v264 & 0xf0000000) >> 30 & 1 | (v264 & 0xf0000000) >> 31 & 1 ^ (v264 & 0xf0000000) >> 28 & 1))
                    {
                        do
                        {
                            v263 *= 0.5;
                            v264 = v264 & 0xfffffff | ((((CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) & (CmpF(v263, 0x463b8000) >> 5 & 3 | CmpF(v263, 0x463b8000) & 1) >> 1 & 1)) * 0x10000000;
                        } while (!((v264 & 0xf0000000) >> 30 & 1 | (v264 & 0xf0000000) >> 31 & 1 ^ (v264 & 0xf0000000) >> 28 & 1));
                        v262 = v256 + v263;
                    }
                    *(v31) = v262;
                    if (!v24)
                    {
                        if (!(*(v30) != 1 || v16[2] != 1))
                        {
                            (&g_20021f70)[idx1] = *((int *)(537009864 + idx1 * 4));
                            if (*((int *)0x2002208c))
                            {
LABEL_80280c1:
                                v265 = v16[3];
                                idx2 = g_20021cbc;
                                goto LABEL_80280c9;
                            }
                        }
                        else if (!(!*((int *)0x2002208c)))
                        {
                            goto LABEL_80280c1;
                        }
                    }
                    else
                    {
                        if (*((int *)0x2002208c))
                        {
                            v265 = v16[3];
                            idx2 = g_20021cbc;
                            if (v24 == 1 && !*(v30))
                            {
                                if (v265 != 1)
                                    goto LABEL_802902b;
                                g_20021de4 = idx2;
LABEL_802902b:
                                v273 = *((int *)v62);
                                (&g_20021f70)[idx1] = *((int *)(537009864 + idx1 * 4));
                                v274 = idx1 + 1;
                                v11 = v273;
                                *(v35) = v274;
                                v275 = *(v38);
                                *(v36) = 0;
                                v276 = (v274 == v11 ? 0 : v274);
                                v270 = v275 + 1;
                                *(v35) = v276;
                                (&g_20021e64)[v276] = 3352888192;
                                *(v38) = v270;
LABEL_80280fb:
                                if (v270 >= v50)
                                    *(v38) = 0;
                                v277 = *((int *)(537010524 + idx2 * 4));
                                v278 = (int)(g_20022154 * *((int *)(v12 + idx2 * 4)) + v221) + *((int *)(537010524 + (idx2 - 1) * 4));
                                *((int *)v59) = v278;
                                v268 = (v277 <= v278 ? v277 - 1 : v278);
                                *((int *)v59) = v268;
                                goto LABEL_8028145;
                            }
LABEL_80280c9:
                            if (v265 != 1 || g_20021de4 == idx2)
                            {
                                v269 = *(v36);
                                v11 = *((int *)v62);
                                v264 = v264 & 0xfffffff | ((((CmpF(v269, v262) >> 5 & 3 | CmpF(v269, v262) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v269, v262) >> 5 & 3 | CmpF(v269, v262) & 1) & (CmpF(v269, v262) >> 5 & 3 | CmpF(v269, v262) & 1) >> 1 & 1)) * 0x10000000;
                                v270 = *(v38);
                                if (!((v264 & 0xf0000000) >> 31 & 1 ^ v264 >> 28 & 1) && (v24 == 1 || v16[2] == 2))
                                {
                                    *(v36) = 0;
                                    v271 = idx1 + 1;
                                    v272 = (v271 == v11 ? 0 : v271);
                                    *(v35) = v272;
                                    (&g_20021e64)[v272] = 3352888192;
                                    v270 += 1;
                                    *(v38) = v270;
                                    goto LABEL_80280fb;
                                }
                            }
                        }
                    }
                    v267 = *(v38);
                    *(v36) = v262;
                    *(v30) = 0;
                    if (v267 >= v50)
                        *(v38) = 0;
                    v11 = *((int *)v62);
                    v268 = 0;
                    *((unsigned int *)v59) = 0;
LABEL_8028145:
                    *(v30) = v24;
                    if (v11 > 0)
                    {
                        v279 = *(v21);
                        v280 = *(v64);
                        v69 = &g_20021de8;
                        v68 = &g_20022070;
                        v281 = 0;
                        v10 = *((int *)0x200220fc);
                        v70 = &g_2000104c;
                        v282 = v264 & 0xfffffff | ((((CmpF(v215, 0) >> 5 & 3 | CmpF(v215, 0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v215, 0) >> 5 & 3 | CmpF(v215, 0) & 1) & (CmpF(v215, 0) >> 5 & 3 | CmpF(v215, 0) & 1) >> 1 & 1)) * 0x10000000;
                        v4 = *((int *)0x20021ca0);
                        v39 = *((int *)0x20001290);
                        v7 = g_20021cbc;
                        v13 = g_2000104c;
                        v264 = v282 & 0xfffffff | ((((CmpF(v279, 1058642330) >> 5 & 3 | CmpF(v279, 1058642330) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v279, 1058642330) >> 5 & 3 | CmpF(v279, 1058642330) & 1) & (CmpF(v279, 1058642330) >> 5 & 3 | CmpF(v279, 1058642330) & 1) >> 1 & 1)) * 0x10000000;
                        v23 = g_20021314;
                        v283 = v268 + g_2002214c;
                        v8 = *((int *)0x20022100);
                        v262 = 0x447a0000;
                        v22 = g_20022070;
                        v26 = g_20021e14;
                        v284 = (unsigned int)*(v20);
                        v42 = *((int *)0x20022098);
                        v41 = g_20021de8;
                        v28 = (((v264 & 0xf0000000) >> 30 | (v264 & 0xf0000000) >> 31 ^ (v264 & 0xf0000000) >> 28) & 1 ? 0 : (((v264 & 0xf0000000) >> 30 | (v264 & 0xf0000000) >> 31 ^ (v264 & 0xf0000000) >> 28) & 1 ^ 1 ? 1 : v283));
                        v285 = *(v63);
                        v3 = 0x20021ee4;
                        v286 = *((int *)0x20022098) * 0.5;
                        v27 = v7 - 1;
                        v71 = v212;
                        v72 = v219;
                        iter = &g_20021e64;
                        iter1 = 0x20021ec8;
                        lr = 537010200;
                        v2 = 0x20021f94;
                        v15 = &g_20021fd8[0];
                        v14 = &g_20021ff8[0];
                        v34 = v280 * 0x447a0000;
                        v29 = 0x447a0000;
                        v46 = v280 * 0x43fa0000;
                        v289 = 0;
                        v290 = 0;
                        v65 = -(v280);
                        index = 0;
                        v292 = _INSERT(v129, 4, (v279 - 1058642330) * *((int *)0x20022098) * 0x411ffbe7);
                        v0 = 0x20021f28;
                        node = 0x20021f08;
                        v9 = *(v17);
                        iter2 = &g_20021f70;
                        do
                        {
                            v294 = *((int *)iter);
                            iter += 4;
                            v295 = v264 & 0xfffffff | ((((CmpF(v294, 3352888192) >> 5 & 3 | CmpF(v294, 3352888192) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v294, 3352888192) >> 5 & 3 | CmpF(v294, 3352888192) & 1) & (CmpF(v294, 3352888192) >> 5 & 3 | CmpF(v294, 3352888192) & 1) >> 1 & 1)) * 0x10000000;
                            if ((v295 & 0xf0000000) >> 30 & 1)
                            {
                                *(iter1) = 0;
                                *(node) = 0;
                                v296 = index * 4;
                                *((unsigned int *)((char *)iter - 4)) = 0;
                                *((unsigned int *)((char *)&g_20021ea8[0] + v296)) = 0;
                                *((unsigned int *)lr) = 3;
                                v297 = *((int *)(537010524 + v7 * 4));
                                v298 = *((int *)(537010524 + v27 * 4));
                                if (v26 == 1)
                                    v284 = v297 - v298;
                                *((unsigned int *)(v14 + index * 4)) = v298;
                                v299 = v295 & 0xfffffff | ((((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) & (CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000;
                                v300 = (unsigned int)(float)(int)(v283 + ((v282 & 0xf0000000) >> 31 & 1 ? 1.0 : (((v282 & 0xf0000000) >> 31 ^ 1) & 1 ? 0 : 1058642330)) * v284);
                                *((unsigned int *)(v15 + index * 4)) = v297;
                                *(v2) = (((v299 & 0xf0000000) >> 30 ^ 1) & 1 ? 0 : ((v299 & 0xf0000000) >> 30 & 1 ? 1 : 3));
                                v301 = (int)v300;
                                *(v0) = v300;
                                v302 = (unsigned int)(float)(int)(v284 - v285);
                                *(iter2) = v302;
                                (&g_20021fb8)[index] = v7;
                                *((unsigned int *)(v6 + v296)) = v284;
                                if (v298 > v301)
                                {
                                    v303 = (unsigned int)(float)(int)((int)v300 + *((int *)(v12 + v7 * 4)));
                                    v301 = (int)v303;
                                    *(v0) = v303;
                                }
                                if (v301 > v297)
                                {
                                    v304 = (unsigned int)(float)(int)((int)v301 - *((int *)(v12 + v7 * 4)));
                                    v301 = (int)v304;
                                    *(v0) = v304;
                                }
                                v305 = 907633515 + 196314165 * v9;
                                v306 = _INSERT(CONCAT(v302, 0), 0, (float)v305);
                                v307 = (int)v306;
                                v308 = 907633515 + 196314165 * v305;
                                v309 = v299 & 0xfffffff | ((((CmpF(v279 - 0.5, v307 * v286) >> 5 & 3 | CmpF(v279 - 0.5, v307 * v286) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v279 - 0.5, v307 * v286) >> 5 & 3 | CmpF(v279 - 0.5, v307 * v286) & 1) & (CmpF(v279 - 0.5, v307 * v286) >> 5 & 3 | CmpF(v279 - 0.5, v307 * v286) & 1) >> 1 & 1)) * 0x10000000;
                                v9 = v308;
                                v310 = v308;
                                if (!((v309 & 0xf0000000) >> 31 & 1 ^ v309 >> 28 & 1))
                                {
                                    v9 = 907633515 + 196314165 * v308;
                                    v311 = v42 * v310;
                                    v310 = v9;
                                }
                                else
                                {
                                    v311 = 0;
                                }
                                *((unsigned int *)(v296 + (char *)&g_20021e88[0])) = v311;
                                v3->field_4 = v28 * *((unsigned int *)((void*)&v306 + 4)) * v310;
                                v312 = v4 & v301;
                                v5 = 3;
                                v13 = 0;
                                goto LABEL_802849d;
                            }
                            else
                            {
                                v301 = *(v0);
                                v297 = *((int *)(v15 + index * 4));
                                v298 = *((int *)(v14 + index * 4));
                                v309 = v295 & 0xfffffff | ((((CmpF(v294, 0x3ff33333) >> 5 & 3 | CmpF(v294, 0x3ff33333) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v294, 0x3ff33333) >> 5 & 3 | CmpF(v294, 0x3ff33333) & 1) & (CmpF(v294, 0x3ff33333) >> 5 & 3 | CmpF(v294, 0x3ff33333) & 1) >> 1 & 1)) * 0x10000000;
                                v312 = v4 & v301;
                                if (!((v309 & 0xf0000000) >> 30 & 1 | (v309 & 0xf0000000) >> 31 & 1 ^ (v309 & 0xf0000000) >> 28 & 1))
                                {
                                    *((unsigned int *)((char *)iter - 4)) = 0;
                                    *((unsigned int *)lr) = 0;
                                    v5 = 0;
LABEL_802849d:
                                    v294 = 0;
                                    goto LABEL_80284a1;
                                }
                                else
                                {
                                    v309 = v309 & 0xfffffff | ((((CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) & (CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                    v5 = *((int *)lr);
                                    if ((v309 & 0xf0000000) >> 30 & 1 | (v309 & 0xf0000000) >> 31 & 1 ^ (v309 & 0xf0000000) >> 28 & 1 || *((int *)(537010524 + v7 * 4)) > v23 && v39)
                                    {
LABEL_80284a1:
                                        if (!v5)
                                            goto LABEL_8028c65;
                                        goto LABEL_80284a9;
                                    }
                                    else
                                    {
                                        v313 = v312 - 1 & v4;
                                        v314 = v312 + 2 & v4;
                                        v315 = *((short *)(v8 + v314 * 2));
                                        v316 = *((short *)(v10 + v314 * 2));
                                        v317 = *(node);
                                        v318 = v312 + 1 & v4;
                                        v319 = *((short *)(v8 + v313 * 2));
                                        v320 = *((short *)(v8 + v312 * 2));
                                        v321 = *((short *)(v10 + v313 * 2));
                                        v322 = *((short *)(v8 + v318 * 2));
                                        v323 = *((short *)(v10 + v312 * 2));
                                        v324 = v317 * 0.5;
                                        v325 = *((short *)(v10 + v318 * 2));
                                        v326 = g_20021e88[index];
                                        v327 = (v320 + (v319 + v322) * 0.5 + (v322 - v319 + (v315 + v319 - v320 - v322) * v324) * v317) * v294;
                                        v328 = 1.0 - v326;
                                        v329 = (v323 + (v321 + v325) * 0.5 + (v325 - v321 + (v316 + v321 - v323 - v325) * v324) * v317) * v294;
                                        v262 = v328 * v327 + v326 * v329;
                                        v281 += v262;
                                        v289 += v326 * v327 + v328 * v329;
                                        if (v5)
                                        {
LABEL_80284a9:
                                            v330 = *(iter2);
                                            v331 = index * 4;
                                            v332 = v6 + (char *)v331;
                                            v333 = *(v332);
                                            v334 = v284 - v285;
                                            v335 = v309 & 0xfffffff | ((((CmpF(v333, v284) >> 5 & 3 | CmpF(v333, v284) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v333, v284) >> 5 & 3 | CmpF(v333, v284) & 1) & (CmpF(v333, v284) >> 5 & 3 | CmpF(v333, v284) & 1) >> 1 & 1)) * 0x10000000;
                                            v262 = (int)v330;
                                            v336 = v335 & 0xfffffff | ((((CmpF(v262, v334) >> 5 & 3 | CmpF(v262, v334) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v262, v334) >> 5 & 3 | CmpF(v262, v334) & 1) & (CmpF(v262, v334) >> 5 & 3 | CmpF(v262, v334) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v335 & 0xf0000000) >> 30 & 1 | (v335 & 0xf0000000) >> 31 & 1 ^ (v335 & 0xf0000000) >> 28 & 1)
                                                goto LABEL_0x80284d5;
                                            *(v332) = v284;
                                            v337 = (unsigned int)(((v336 & 0xf0000000) >> 30 | (v336 & 0xf0000000) >> 31 ^ (v336 & 0xf0000000) >> 28) & 1 ^ 1 ? (float)(int)v334 : v330);
                                            v338 = v336 & 0xfffffff | ((((CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) & (CmpF(v294, 0.0) >> 5 & 3 | CmpF(v294, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v336 & 0xf0000000) >> 30 & 1 | (v336 & 0xf0000000) >> 31 & 1 ^ (v336 & 0xf0000000) >> 28 & 1)
                                                goto LABEL_0x80284e9;
                                            *(iter2) = v337;
                                            v339 = *(iter1);
                                            if ((v338 & 0xf0000000) < 0)
                                            {
                                                v341 = 0;
                                                v340 = (float)(v292 - 841731191);
                                            }
                                            else if (v339 > (int)v337)
                                            {
                                                v340 = (float)(v294 - v280);
                                                v292 = v65;
                                                v341 = 1;
                                            }
                                            else
                                            {
                                                v338 = v338 & 0xfffffff | ((((CmpF(v294, 1.0) >> 5 & 3 | CmpF(v294, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v294, 1.0) >> 5 & 3 | CmpF(v294, 1.0) & 1) & (CmpF(v294, 1.0) >> 5 & 3 | CmpF(v294, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                                                if ((v338 & 0xf0000000) >> 31 & 1 ^ v338 >> 28 & 1 || v5 != 3)
                                                {
                                                    v340 = (float)(v280 + v294);
                                                    v292 = v280;
                                                    v341 = 3;
                                                }
                                                else
                                                {
                                                    v292 = 0;
                                                    v340 = (float)1.0;
                                                    v341 = 2;
                                                }
                                            }
                                            v342 = v338 & 0xfffffff | ((((CmpF(v284, v29) >> 5 & 3 | CmpF(v284, v29) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v284, v29) >> 5 & 3 | CmpF(v284, v29) & 1) & (CmpF(v284, v29) >> 5 & 3 | CmpF(v284, v29) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v342 & 0xf0000000) >> 31 & 1)
                                            {
                                                if (v13)
                                                    goto LABEL_8028cc1;
                                            }
                                            else
                                            {
                                                if (v339 + 749 < (int)v337 || v13 || v341 != 2 || (v342 = v342 & 0xfffffff | ((((CmpF((unsigned long long)v285, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v285, 0x437a0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v285, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v285, 0x437a0000) & 1) & (CmpF((unsigned long long)v285, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v285, 0x437a0000) & 1) >> 1 & 1)) * 0x10000000, !((v342 & 0xf0000000) >> 30 & 1) || (v342 = v342 & 0xfffffff | ((((CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) & (CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000, (v342 & 0xf0000000) >> 30 & 1 || *(v2))))
                                                {
LABEL_8028cc1:
                                                    v342 = v342 & 0xfffffff | ((((CmpF(v34, (unsigned int)v340) >> 5 & 3 | CmpF(v34, (unsigned int)v340) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v34, (unsigned int)v340) >> 5 & 3 | CmpF(v34, (unsigned int)v340) & 1) & (CmpF(v34, (unsigned int)v340) >> 5 & 3 | CmpF(v34, (unsigned int)v340) & 1) >> 1 & 1)) * 0x10000000;
                                                    if (!((v342 & 0xf0000000) >> 30 & 1 | (v342 & 0xf0000000) >> 31 & 1 ^ (v342 & 0xf0000000) >> 28 & 1))
                                                    {
                                                        v342 = v342 & 0xfffffff | ((((CmpF(v46, (unsigned int)v340) >> 5 & 3 | CmpF(v46, (unsigned int)v340) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v46, (unsigned int)v340) >> 5 & 3 | CmpF(v46, (unsigned int)v340) & 1) & (CmpF(v46, (unsigned int)v340) >> 5 & 3 | CmpF(v46, (unsigned int)v340) & 1) >> 1 & 1)) * 0x10000000;
                                                        if ((v342 & 0xf0000000) >> 31 & 1 && !v13 && v341 == 1)
                                                        {
                                                            v342 = v342 & 0xfffffff | ((((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) & (CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000;
                                                            if (!((v342 & 0xf0000000) >> 30 & 1))
                                                                v41 = (!*(v2) ? 4 : v41);
                                                        }
                                                    }
LABEL_8028525:
                                                    if (v298 >= v312 - 63)
                                                        v340 = (float)(v340 * (v312 - v298) * 0x3c800000);
                                                    if (v312 + 63 >= v297)
                                                        v340 = (float)(v340 * (v297 - v312) * 0x3c800000);
                                                    *((unsigned int *)lr) = v341;
                                                    v343 = v331 + &g_20021ea8[0];
                                                    v344 = *(v343);
                                                    *((float *)((char *)iter - 4)) = v340;
                                                    v345 = v224 + v344;
                                                    v346 = v25 + v339;
                                                    v309 = v342 & 0xfffffff | ((((CmpF(v345, 1.0) >> 5 & 3 | CmpF(v345, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v345, 1.0) >> 5 & 3 | CmpF(v345, 1.0) & 1) & (CmpF(v345, 1.0) >> 5 & 3 | CmpF(v345, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                                                    *(iter1) = v346;
                                                    if (!((v309 & 0xf0000000) >> 31 & 1 ^ v309 >> 28 & 1))
                                                    {
                                                        *(v343) = v345 - 1.0;
                                                        *(iter1) = v346 + 1;
                                                        goto LABEL_8028597;
                                                    }
                                                    else
                                                    {
                                                        v309 = v309 & 0xfffffff | ((((CmpF(v345, 0.0) >> 5 & 3 | CmpF(v345, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v345, 0.0) >> 5 & 3 | CmpF(v345, 0.0) & 1) & (CmpF(v345, 0.0) >> 5 & 3 | CmpF(v345, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                                        if ((v309 & 0xf0000000) >= 0)
                                                        {
                                                            *(v343) = v345;
                                                            goto LABEL_8028597;
                                                        }
                                                        else
                                                        {
                                                            if (!1)
                                                                goto LABEL_0x8028ff3;
                                                            *(v343) = v345 + 1.0;
                                                            *(iter1) = v346 - 1;
                                                            if (!1)
                                                                goto LABEL_8028ffb;
                                                            else
                                                                goto LABEL_8028597;
                                                        }
                                                    }
                                                }
                                            }
                                            v41 = 4;
                                            goto LABEL_8028525;
                                        }
                                        else
                                        {
LABEL_8028c65:
                                            v340 = 2989214839;
                                            v301 = *(v0);
                                            *((unsigned int *)((char *)iter - 4)) = 2989214839;
                                        }
                                    }
                                }
                            }
LABEL_8028597:
                            v3 = &v3->field_4;
                            v347 = v309 & 0xfffffff | ((((CmpF(v290, (unsigned int)v340) >> 5 & 3 | CmpF(v290, (unsigned int)v340) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v290, (unsigned int)v340) >> 5 & 3 | CmpF(v290, (unsigned int)v340) & 1) & (CmpF(v290, (unsigned int)v340) >> 5 & 3 | CmpF(v290, (unsigned int)v340) & 1) >> 1 & 1)) * 0x10000000;
                            v348 = &(&v86)[4 * v3->field_4];
                            v290 = (unsigned int)((v347 & 0xf0000000) >> 31 & 1 ? v340 : v290);
                            v349 = *((int *)((char *)v348 - 16));
                            v350 = *(node);
                            v351 = *((int *)((char *)v348 - 32));
                            v22 = index;
                            v352 = v350 + v349;
                            v353 = v301 + v351;
                            v264 = v347 & 0xfffffff | ((((CmpF(v352, 1.0) >> 5 & 3 | CmpF(v352, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v352, 1.0) >> 5 & 3 | CmpF(v352, 1.0) & 1) & (CmpF(v352, 1.0) >> 5 & 3 | CmpF(v352, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v0) = v353;
                            if (!((v264 & 0xf0000000) >> 31 & 1 ^ v264 >> 28 & 1))
                            {
                                v354 = v353 + 1;
                                *(node) = v352 - 1.0;
                            }
                            else
                            {
                                v264 = v264 & 0xfffffff | ((((CmpF(v352, 0.0) >> 5 & 3 | CmpF(v352, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v352, 0.0) >> 5 & 3 | CmpF(v352, 0.0) & 1) & (CmpF(v352, 0.0) >> 5 & 3 | CmpF(v352, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                v354 = ((v264 & 0xf0000000) >> 31 & 1 ? v353 - 1 : v353);
                                *(node) = ((v264 & 0xf0000000) >> 31 & 1 ? v352 + 1.0 : v352);
                            }
                            if (v297 >= v354)
                                v298 = (v298 <= v354 ? v354 : (v354 < v298 ? v297 : v298));
                            *(v0) = v298;
                            index += 1;
                            v0 += 1;
                            node += 1;
                            iter1 += 1;
                            lr = lr + 4;
                            v2 += 1;
                            iter2 += 1;
                        } while (index != v11);
                        *((unsigned int *)v20) = v284;
                        *((unsigned int *)v70) = v13;
                        *((unsigned int *)v69) = v41;
                        v219 = v72;
                        *(v17) = v9;
                        v212 = v71;
                        *((unsigned int *)v68) = v22;
                    }
                    else
                    {
                        v281 = 0;
                        v289 = 0;
                    }
                    v355 = v73 + *(v54) * 1065185444;
                    v356 = *((int *)v51);
                    *(v54) = v355;
                    if (1000 > v356)
                    {
                        v357 = v355 * 0x38000100;
                        v358 = v289 * v357;
                        v359 = v281 * v357;
                    }
                    else
                    {
LABEL_8028ffb:
                        v359 = 0;
                        v358 = 0;
                    }
                    v360 = *(v32);
                    v361 = *(v44);
                    v362 = v264 & 0xfffffff | ((((CmpF(v360, 0.0) >> 5 & 3 | CmpF(v360, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v360, 0.0) >> 5 & 3 | CmpF(v360, 0.0) & 1) & (CmpF(v360, 0.0) >> 5 & 3 | CmpF(v360, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                    v363 = *(v33) * 0x3f7fbe77;
                    if ((v362 & 0xf0000000) >> 30 & 1)
                        goto LABEL_0x8028e33;
                    v364 = (((v362 & 0xf0000000) >> 30 ^ 1) & 1 ? v363 + 981668463 : v363);
                    v365 = v358 * v364;
                    v366 = v359 * v364;
                    v367 = v362 & 0xfffffff | ((((CmpF(v365, 1.0) >> 5 & 3 | CmpF(v365, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v365, 1.0) >> 5 & 3 | CmpF(v365, 1.0) & 1) & (CmpF(v365, 1.0) >> 5 & 3 | CmpF(v365, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v368 = v367 & 0xfffffff | ((((CmpF(v366, 1.0) >> 5 & 3 | CmpF(v366, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v366, 1.0) >> 5 & 3 | CmpF(v366, 1.0) & 1) & (CmpF(v366, 1.0) >> 5 & 3 | CmpF(v366, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v369 = (v367 & 0xf0000000 & 0x20000000 && !(v367 & 0xf0000000 & 0x40000000) ? 1.0 : v365);
                    v370 = (v368 & 0xf0000000 & 0x20000000 && !(v368 & 0xf0000000 & 0x40000000) ? 1.0 : v366);
                    v371 = v368 & 0xfffffff | ((((CmpF(v369, -1.0) >> 5 & 3 | CmpF(v369, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v369, -1.0) >> 5 & 3 | CmpF(v369, -1.0) & 1) & (CmpF(v369, -1.0) >> 5 & 3 | CmpF(v369, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v372 = v371 & 0xfffffff | ((((CmpF(v370, -1.0) >> 5 & 3 | CmpF(v370, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v370, -1.0) >> 5 & 3 | CmpF(v370, -1.0) & 1) & (CmpF(v370, -1.0) >> 5 & 3 | CmpF(v370, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v373 = v219 + v18->field_0 * ((((v372 & 0xf0000000) >> 3 ^ v372 & 0xf0000000) & 0x10000000 ? -1.0 : v370) - v219);
                    v374 = v212 + ((((v371 & 0xf0000000) >> 3 ^ v371 & 0xf0000000) & 0x10000000 ? -1.0 : v369) - v212) * v18->field_0;
                    v375 = (!v16[9] ? v373 : v219);
                    v376 = (!v16[9] ? v374 : v212);
                    *(v33) = v364;
                    v377 = v373;
                    if (v361 == g_20021de4)
                    {
                        v262 = -(g_20024130) + g_20024138 * 1065302884;
                        v378 = v376 * 1060320051;
                        v379 = v375 * 1060320051;
                        v376 = -(g_2002412c) + g_20024134 * 1065302884 + v378;
                        v375 = v262 + v379;
                        g_2002412c = v378;
                        g_20024134 = v376;
                        g_20024138 = v375;
                        g_20024130 = v379;
                    }
                    if (*(v43) != 1)
                    {
LABEL_8028eff:
                        if (*(v40) == 1)
                            goto LABEL_8029279;
                        goto LABEL_8028f09;
                    }
                    v380 = *(0x20021ca4);
                    g_200213b4 = 0;
                    if (*(0x20021ca4) > g_2002210c)
                    {
                        *((short *)(*((int *)0x200220fc) + g_2002210c * 2)) = v376 * 0x46fffe00;
                        *((short *)(*((int *)0x20022100) + g_2002210c * 2)) = v375 * 0x46fffe00;
                    }
                    v381 = *((int *)537010316);
                    v382 = g_2002210c + 1;
                    g_2002210c = v382;
                    if (!*((int *)0x20001290))
                    {
                        if (v361 <= *((int *)537010316))
                            goto LABEL_8029413;
                        if (*((int *)537009316) > v382)
                            goto LABEL_8029235;
                        v0 = g_20021314;
                        g_20021c9c = 0;
                        g_20021320 = 0;
                        *(v43) = 0;
                        g_20021314 = v382;
                        sub_8026619();
                        *(v53) = 1;
                        g_20021328 = (*((int *)0x20001290) == 1 ? g_20022074 | 2176 : g_20022074);
                        v380 = *(0x20021ca4);
                        node = *((int *)0x20021c80);
                        v0 = g_20021c6c;
                        v383 = g_20021c78 + 100;
                        g_200220a4 = *((int *)0x20021c68) + 100;
                        g_200220a8 = v0 + 100;
                        g_200220b4 = v383;
                        g_200220ac = *((int *)0x20021c70) + 100;
                        g_200220b0 = g_20021c74 + 100;
                        g_200220b8 = g_20021c7c + 100;
                        v384 = *((int *)0x20021c84) + 100;
                        g_200220bc = node + 100;
                        g_200220c0 = v384;
                        v361 = *(v44);
                        v381 = *(0x2002208c);
                        v382 = g_2002210c;
                        *(v77) = 480;
                    }
                    if (v361 > v381)
                        goto LABEL_80293ed;
LABEL_8029413:
                    if (v382 >= *((int *)(537010524 + v361 * 4)))
                    {
                        v382 = *((int *)(537010524 + (v361 - 1) * 4));
                        g_2002210c = v382;
                    }
LABEL_80293ed:
                    if (v382 >= v380)
                    {
                        v385 = *(v75);
                        g_200213b8 = 2000;
                        g_2002133c = 2000;
                        sub_802734d((v381 < v361 ? 1 : (v361 <= v381 ? 0 : v382)), v385);
                    }
LABEL_8029235:
                    if (g_20021320 || *(v43) != 1 || !v76 && g_20021cb0)
                        goto LABEL_8028eff;
                    v386 = *(v44);
                    sub_802734d((*((int *)537010316) < v386 ? 1 : (v386 <= *((int *)537010316) ? 0 : v386)), *(v75), v75);
                    if (*(v40) != 1)
                    {
LABEL_8028f09:
                        if (*(v45) == 1 && g_200213b4)
                        {
                            g_200213ac = 1000;
                            sub_8026619(g_200213b4, &g_200213ac);
                            *(v45) = 0;
                            *(v40) = 0;
                            g_200213b4 = 0;
                            goto LABEL_8028f1b;
                        }
                    }
                    else
                    {
LABEL_8029279:
                        g_200213b4 = *((int *)(537009960 + g_20022070 * 4));
                        *(v40) = 2;
                        *(v45) = 0;
LABEL_8028f1b:
                        v387 = v19;
                        v388 = (unsigned int)*(v387);
                        v389 = v372 & 0xfffffff | ((((CmpF(v377, 0.0) >> 5 & 3 | CmpF(v377, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v377, 0.0) >> 5 & 3 | CmpF(v377, 0.0) & 1) & (CmpF(v377, 0.0) >> 5 & 3 | CmpF(v377, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                        v390 = UnaryOp Abs;
                        v391 = (((v389 & 0xf0000000) >> 31 ^ 1) & 1 ? v377 + v390 : ((v389 & 0xf0000000) >> 31 & 1 ? v390 - v377 : v262));
                        if (!v16[5])
                        {
                            v392 = v389 & 0xfffffff | ((((CmpF(v391, v388) >> 5 & 3 | CmpF(v391, v388) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v391, v388) >> 5 & 3 | CmpF(v391, v388) & 1) & (CmpF(v391, v388) >> 5 & 3 | CmpF(v391, v388) & 1) >> 1 & 1)) * 0x10000000;
                            if (!((v392 & 0xf0000000) >> 30 & 1 | (v392 & 0xf0000000) >> 31 & 1 ^ (v392 & 0xf0000000) >> 28 & 1))
                            {
                                v393 = v387;
                                v394 = (float)(v388 * 1063675494 + v391 * 1036831949);
                            }
                            else
                            {
                                v393 = v387;
                                v394 = (float)(v388 * 0x3f7fbe77 + v391 * 981668463);
                            }
                            v191 = v392 & 0xfffffff | ((((CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) & (CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v393) = v394;
                            if ((v191 & 0xf0000000) >> 30 & 1 | (v191 & 0xf0000000) >> 31 & 1 ^ (v191 & 0xf0000000) >> 28 & 1)
                                goto LABEL_80290b5;
                            goto LABEL_8028f6f;
                        }
                        else
                        {
                            v394 = (float)(v388 * 1063675494 + *((int *)(537009864 + *(v35) * 4)) * g_20021e84 * g_2002209c * 1031127695);
                            v191 = v389 & 0xfffffff | ((((CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) & (CmpF((unsigned int)v394, 1.0) >> 5 & 3 | CmpF((unsigned int)v394, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v19) = v394;
                            if (!((v191 & 0xf0000000) >> 30 & 1 | (v191 & 0xf0000000) >> 31 & 1 ^ (v191 & 0xf0000000) >> 28 & 1))
                            {
LABEL_8028f6f:
                                v394 = (float)1.0;
                                *((unsigned int *)v19) = 1.0;
                                goto LABEL_8028f79;
                            }
                            else
                            {
LABEL_80290b5:
                                v191 = v191 & 0xfffffff | ((((CmpF((unsigned int)v394, 0.0) >> 5 & 3 | CmpF((unsigned int)v394, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v394, 0.0) >> 5 & 3 | CmpF((unsigned int)v394, 0.0) & 1) & (CmpF((unsigned int)v394, 0.0) >> 5 & 3 | CmpF((unsigned int)v394, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                if ((v191 & 0xf0000000) >> 31 & 1)
                                {
                                    v394 = 0;
                                    *((unsigned int *)v19) = 0;
                                }
LABEL_8028f79:
                                v395 = *(v60);
                                v396 = *(v61);
                                *(v60) = v395 + 1 & 63;
                                v397 = a1;
                                *((float *)(v57 + v395 * 4)) = v394;
                                *((short *)(v397 + v66)) = v374 * 0x46fffe00;
                                *((short *)(v397 + v67)) = v377 * 0x46fffe00;
                                *(v61) = v396 + 2;
                                v37 = (unsigned short)v37 + 2;
                                if (v47 <= v37)
                                    break;
                                v194 = v18->field_10;
                                v188 = g_20022154;
                                v193 = v37;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (v192 > 0)
        {
            v38 = &g_20022038;
            v35 = &g_20021cb8;
            v44 = &g_20022114;
            v32 = &g_200220e8;
            v30 = &g_200220ec;
            v16 = 536875688;
            v43 = &g_20022108;
            v40 = &g_200213a8;
            v58 = &g_200220e0;
            v21 = &g_20022084;
            v31 = &g_20021e20;
            v55 = &g_200220d4;
            v17 = &g_200220f8;
            v56 = 0x20022124;
            v53 = &g_20001054;
            v36 = &g_20021e18;
            v6 = &g_20021f4c[0];
            v62 = 537010312;
            v60 = &g_20021dc0;
            v59 = &g_20022158;
            v398 = 0;
            v54 = &g_20022064;
            v33 = &g_20022090;
            v45 = &g_200213c0;
            v19 = &g_200220c8;
            v57 = &g_20021cc0;
            v61 = &g_20021c88;
            v67 = v171 * 1008981770;
            v68 = 1008981770;
            v28 = 0;
            v399 = v129;
            v400 = v163;
            while (1)
            {
                v401 = a0;
                v402 = *(v55);
                v403 = *(v58);
                v404 = *(v32);
                v41 = v398 * 2;
                v405 = v18;
                v42 = v41 + 2;
                v406 = *((short *)(v401 + v398 * 2)) * 0x38000100;
                v407 = UnaryOp Abs;
                v408 = v406 * 1.5;
                v409 = v191 & 0xfffffff | ((((CmpF(v407, v402) >> 5 & 3 | CmpF(v407, v402) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v407, v402) >> 5 & 3 | CmpF(v407, v402) & 1) & (CmpF(v407, v402) >> 5 & 3 | CmpF(v407, v402) & 1) >> 1 & 1)) * 0x10000000;
                v410 = *((short *)(v401 + v42)) * 0x38000100;
                v411 = v409 & 0xfffffff | ((((CmpF(v408, 1.0) >> 5 & 3 | CmpF(v408, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v408, 1.0) >> 5 & 3 | CmpF(v408, 1.0) & 1) & (CmpF(v408, 1.0) >> 5 & 3 | CmpF(v408, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                v412 = v405->field_0;
                v413 = (v411 & 0xf0000000 & 0x20000000 && !(v411 & 0xf0000000 & 0x40000000) ? 1.0 : v408);
                v414 = v410 * 1.5;
                v415 = v411 & 0xfffffff | ((((CmpF(v413, -1.0) >> 5 & 3 | CmpF(v413, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v413, -1.0) >> 5 & 3 | CmpF(v413, -1.0) & 1) & (CmpF(v413, -1.0) >> 5 & 3 | CmpF(v413, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                v416 = *(v21) + (v155 - *(v21)) * v68;
                v417 = 907633515 + 196314165 * *(v17);
                *(v17) = v417;
                v418 = v415 & 0xfffffff | ((((CmpF(v414, 1.0) >> 5 & 3 | CmpF(v414, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v414, 1.0) >> 5 & 3 | CmpF(v414, 1.0) & 1) & (CmpF(v414, 1.0) >> 5 & 3 | CmpF(v414, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                v419 = (((v415 & 0xf0000000) >> 3 ^ v415 & 0xf0000000) & 0x10000000 ? -1.0 : v413);
                v420 = (v418 & 0xf0000000 & 0x20000000 && !(v418 & 0xf0000000 & 0x40000000) ? 1.0 : v414);
                v421 = v403 + (v404 - v403) * v48;
                v422 = (((v409 & 0xf0000000) >> 3 ^ v409 & 0xf0000000) & 0x10000000 ? v402 : v407) * 0x3f7fffac;
                v423 = v418 & 0xfffffff | ((((CmpF(v420, -1.0) >> 5 & 3 | CmpF(v420, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v420, -1.0) >> 5 & 3 | CmpF(v420, -1.0) & 1) & (CmpF(v420, -1.0) >> 5 & 3 | CmpF(v420, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                *(v55) = v422;
                v424 = v423 & 0xfffffff | ((((CmpF(v416, 0.25) >> 5 & 3 | CmpF(v416, 0.25) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v416, 0.25) >> 5 & 3 | CmpF(v416, 0.25) & 1) & (CmpF(v416, 0.25) >> 5 & 3 | CmpF(v416, 0.25) & 1) >> 1 & 1)) * 0x10000000;
                g_20022154 = v188 + (v400 - v188) * v52;
                v425 = (((v423 & 0xf0000000) >> 3 ^ v423 & 0xf0000000) & 0x10000000 ? -1.0 : v420);
                v405->field_0 = v412 + (v49 - v412) * 981668463;
                *(v58) = v421;
                *(v21) = v416;
                if (v424 & 0x20000000 && !(v424 & 0x40000000) && !(v0 = g_20021c7c, v426 = (unsigned int)(int)(long long)(int)v0, v424 = v424 & 0xfffffff | ((((CmpF((unsigned long long)v426, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)v426, 0x44fa0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) & 1) & (CmpF((unsigned long long)v426, 0x44fa0000) >> 5 & 3 | CmpF((unsigned long long)(unsigned int)(int)(long long)(int)v0, 0x44fa0000) & 1) >> 1 & 1)) * 0x10000000, (v424 & 0xf0000000) < 0))
                    v427 = (-0.5 + v417 * *((int *)0x20022098)) * 0x461c4000 * v416 * v416;
                else
                    v427 = 0;
                if (*((int *)(v16 + 32)))
                {
                    v424 = v424 & 0xfffffff | ((((CmpF(v422, 981668463) >> 5 & 3 | CmpF(v422, 981668463) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v422, 981668463) >> 5 & 3 | CmpF(v422, 981668463) & 1) & (CmpF(v422, 981668463) >> 5 & 3 | CmpF(v422, 981668463) & 1) >> 1 & 1)) * 0x10000000;
                    v421 = ((v424 & 0xf0000000) >> 31 & 1 ? v421 + v410 * ((v424 & 0xf0000000) >> 31 & 1 ? 3.0 : 981668463) : v421);
                }
                v428 = (unsigned int)(float)(int)v421;
                v429 = v421 - (int)v428;
                if (g_20021e14 != 1)
                {
                    v430 = 0;
                    v15 = 1;
                }
                else
                {
                    v424 = v424 & 0xfffffff | ((((CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) & (CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v424 & 0xf0000000) >> 30 & 1 | (v424 & 0xf0000000) >> 31 & 1 ^ (v424 & 0xf0000000) >> 28 & 1))
                    {
                        v430 = v429;
                        v15 = v428;
                    }
                    else
                    {
                        v431 = -(v421);
                        v15 = (unsigned int)(float)(int)v431;
                        v430 = v431 - (int)v15;
                    }
                }
                v432 = v56;
                v78 = v428;
                v82 = v429;
                v433 = v421 * *(v432);
                v434 = v421 * v432[1];
                v435 = v421 * v432[2];
                v436 = (unsigned int)(float)(int)v433;
                v437 = (unsigned int)(float)(int)v434;
                v438 = (unsigned int)(float)(int)v435;
                v439 = (int)v436;
                v440 = v433 - v439;
                v441 = v434 - (int)v437;
                v442 = v435 - (int)v438;
                v443 = v424 & 0xfffffff | ((((CmpF(v429, 0.0) >> 5 & 3 | CmpF(v429, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v429, 0.0) >> 5 & 3 | CmpF(v429, 0.0) & 1) & (CmpF(v429, 0.0) >> 5 & 3 | CmpF(v429, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                v79 = v436;
                v83 = v440;
                v80 = v437;
                v84 = v441;
                v81 = v438;
                v85 = v442;
                if ((v443 & 0xf0000000) >> 31 & 1)
                {
                    v82 = v429 + 1.0;
                    v78 = (int)v428 - 1;
                }
                v444 = v443 & 0xfffffff | ((((CmpF(v440, 0.0) >> 5 & 3 | CmpF(v440, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v440, 0.0) >> 5 & 3 | CmpF(v440, 0.0) & 1) & (CmpF(v440, 0.0) >> 5 & 3 | CmpF(v440, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v444 & 0xf0000000) >> 31 & 1)
                {
                    v83 = v440 + 1.0;
                    v79 = (int)v436 - 1;
                }
                v445 = v444 & 0xfffffff | ((((CmpF(v441, 0.0) >> 5 & 3 | CmpF(v441, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v441, 0.0) >> 5 & 3 | CmpF(v441, 0.0) & 1) & (CmpF(v441, 0.0) >> 5 & 3 | CmpF(v441, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v445 & 0xf0000000) >> 31 & 1)
                {
                    v84 = v441 + 1.0;
                    v80 = (int)v437 - 1;
                }
                v446 = v445 & 0xfffffff | ((((CmpF(v442, 0.0) >> 5 & 3 | CmpF(v442, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v442, 0.0) >> 5 & 3 | CmpF(v442, 0.0) & 1) & (CmpF(v442, 0.0) >> 5 & 3 | CmpF(v442, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                if ((v446 & 0xf0000000) >> 31 & 1)
                {
                    v85 = v442 + 1.0;
                    v81 = (int)v438 - 1;
                }
                if (g_20021cb0 != 1)
                {
LABEL_80297cd:
                    v448 = (unsigned int)*(v20);
                    if (g_20021ca8 != 1 || v28)
                    {
                        v447 = g_20021f48;
                        goto LABEL_80297e3;
                    }
                    v449 = (int)g_2002214c;
                    v450 = (g_20021c74 < 2000 ? v449 - v448 : v449);
                    v451 = *((int *)(v12 + g_20021de4 * 4));
                    *((float *)&g_2002214c) = (float)(int)(2000 <= g_20021c74 ? v450 + v448 : v450);
                    v452 = (int)g_2002214c;
                    v446 = v446 & 0xfffffff | ((((CmpF(v452, v451) >> 5 & 3 | CmpF(v452, v451) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v452, v451) >> 5 & 3 | CmpF(v452, v451) & 1) & (CmpF(v452, v451) >> 5 & 3 | CmpF(v452, v451) & 1) >> 1 & 1)) * 0x10000000;
                    if ((v446 & 0xf0000000) >> 30 & 1 | (v446 & 0xf0000000) >> 31 & 1 ^ (v446 & 0xf0000000) >> 28 & 1)
                    {
                        v446 = v446 & 0xfffffff | ((((CmpF(v452, 0.0) >> 5 & 3 | CmpF(v452, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v452, 0.0) >> 5 & 3 | CmpF(v452, 0.0) & 1) & (CmpF(v452, 0.0) >> 5 & 3 | CmpF(v452, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                        if ((v446 & 0xf0000000) >> 31 & 1)
                            *((float *)&g_2002214c) = (float)(int)(v451 - 1.0);
                    }
                    else
                    {
                        g_2002214c = 0;
                    }
                    v453 = *((int *)v53);
                    if (g_20021e14 != 1)
                    {
                        *(v36) = *(v31);
LABEL_802a5eb:
                        v447 = g_20021f48;
                        if (v453 == 1)
                        {
LABEL_802a497:
                            v454 = *(v36);
                            v448 = (unsigned int)*(v20);
                            goto LABEL_802a60d;
                        }
                        else
                        {
LABEL_802a5f7:
                            v448 = (unsigned int)*(v20);
                            goto LABEL_80297ed;
                        }
                    }
                    else
                    {
LABEL_802a7d5:
                        v447 = g_20021f48;
                        if (v453 == 1)
                        {
                            v454 = *(v36);
LABEL_802a7e7:
                            v446 = v446 & 0xfffffff | ((((CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) & (CmpF(v421, 0.0) >> 5 & 3 | CmpF(v421, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                            if ((v446 & 0xf0000000) < 0)
                            {
LABEL_802ab5f:
                                *(v36) = v454 - v421;
                                goto LABEL_80297ed;
                            }
                            else if (v446 & 0x40000000 || (v446 & 0xf0000000) >> 3 >> 28 & 1 ^ v446 >> 28 & 1)
                            {
LABEL_802a60d:
                                *(v36) = v454 + 1.0;
                                goto LABEL_80297ed;
                            }
                            else
                            {
                                *(v36) = v421 + v454;
                                goto LABEL_80297ed;
                            }
                        }
                    }
                }
                else
                {
                    v447 = g_20021f48;
                    v446 = v446 & 0xfffffff | ((((CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) & (CmpF(g_20021f48, 0.5) >> 5 & 3 | CmpF(g_20021f48, 0.5) & 1) >> 1 & 1)) * 0x10000000;
                    if ((v446 & 0xf0000000) >> 31 & 1 && *((int *)(v16 + 24)) != 1)
                    {
                        if (g_20021e14)
                            goto LABEL_802a783;
                        goto LABEL_802a413;
                    }
                    if (*((int *)(v16 + 24)) != 2)
                        goto LABEL_80297cd;
                    if (!g_20021e14)
                    {
LABEL_802a413:
                        v439 = g_20022144;
                        v455 = (g_20021c74 < 2000 ? g_20022148 - g_20022144 : g_20022148);
                        v456 = (2000 <= g_20021c74 ? v455 + g_20022144 : v455);
                        v457 = (unsigned int)(float)(int)v456;
                        v458 = (int)v457;
                        v459 = g_2002214c + v458;
                        g_20022148 = v456 - (int)v457;
                        g_2002214c = v459;
                        if (v458 < 0)
                        {
LABEL_802aa6d:
                            v459 -= 1;
                            g_20022148 = g_20022148 + 1.0;
                            g_2002214c = v459;
                        }
                        v460 = *((int *)(v12 + g_20021de4 * 4));
                        v453 = *((int *)v53);
                        v461 = v459;
                        v446 = v446 & 0xfffffff | ((((CmpF(v461, v460) >> 5 & 3 | CmpF(v461, v460) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v459, v460) >> 5 & 3 | CmpF(v459, v460) & 1) & (CmpF(v459, v460) >> 5 & 3 | CmpF(v459, v460) & 1) >> 1 & 1)) * 0x10000000;
                        if (!((v446 & 0xf0000000) >> 30 & 1 | (v446 & 0xf0000000) >> 31 & 1 ^ (v446 & 0xf0000000) >> 28 & 1))
                        {
                            g_2002214c = 0;
                            goto LABEL_802a5eb;
                        }
                        else if (0 > v459)
                        {
                            *((float *)&g_2002214c) = (float)(int)(v460 - 1.0);
                            v447 = g_20021f48;
                            if (v453 != 1)
                                goto LABEL_802a5f7;
                            goto LABEL_802a497;
                        }
                    }
LABEL_802a783:
                    v448 = (unsigned int)*(v20);
LABEL_80297e3:
                    if (*((int *)v53) == 1)
                    {
                        v454 = *(v36);
                        if (g_20021e14 == 1)
                            goto LABEL_802a7e7;
                        goto LABEL_802a60d;
                    }
LABEL_80297ed:
                    v462 = *(v35);
                    v463 = *((int *)(v6 + v462 * 4));
                    v464 = v447 * v448;
                    v465 = v463 * v447;
                    v466 = v446 & 0xfffffff | ((((CmpF(v465, v464) >> 5 & 3 | CmpF(v465, v464) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v465, v464) >> 5 & 3 | CmpF(v465, v464) & 1) & (CmpF(v465, v464) >> 5 & 3 | CmpF(v465, v464) & 1) >> 1 & 1)) * 0x10000000;
                    v467 = (((v466 & 0xf0000000) >> 30 | (v466 & 0xf0000000) >> 31 ^ (v466 & 0xf0000000) >> 28) & 1 ? v465 : v464);
                    v468 = v466 & 0xfffffff | ((((CmpF(v467, 1.0) >> 5 & 3 | CmpF(v467, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v467, 1.0) >> 5 & 3 | CmpF(v467, 1.0) & 1) & (CmpF(v467, 1.0) >> 5 & 3 | CmpF(v467, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v466 & 0xf0000000) >> 30 & 1 | (v466 & 0xf0000000) >> 31 & 1 ^ (v466 & 0xf0000000) >> 28 & 1))
                        goto LABEL_0x80298cd;
                    *(v31) = v465;
                    if ((v466 & 0xf0000000) >> 30 & 1 | (v466 & 0xf0000000) >> 31 & 1 ^ (v466 & 0xf0000000) >> 28 & 1)
                        goto LABEL_80298d1;
                    else
                        goto LABEL_80298d1;
                    *(v31) = v467;
LABEL_80298d1:
                    v469 = (((v468 & 0xf0000000) >> 30 | (v468 & 0xf0000000) >> 31 ^ (v468 & 0xf0000000) >> 28) & 1 ^ 1 ? v467 : (((v468 & 0xf0000000) >> 30 | (v468 & 0xf0000000) >> 31 ^ (v468 & 0xf0000000) >> 28) & 1 ? 1.0 : v439));
                    v470 = v469 - v463;
                    v471 = v468 & 0xfffffff | ((((CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) & (CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) >> 1 & 1)) * 0x10000000;
                    if (!((v471 & 0xf0000000) >> 30 & 1 | (v471 & 0xf0000000) >> 31 & 1 ^ (v471 & 0xf0000000) >> 28 & 1))
                    {
                        do
                        {
                            v470 *= 0.5;
                            v471 = v471 & 0xfffffff | ((((CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) & (CmpF(v470, 0x463b8000) >> 5 & 3 | CmpF(v470, 0x463b8000) & 1) >> 1 & 1)) * 0x10000000;
                        } while (!((v471 & 0xf0000000) >> 30 & 1 | (v471 & 0xf0000000) >> 31 & 1 ^ (v471 & 0xf0000000) >> 28 & 1));
                        v469 = v463 + v470;
                    }
                    *(v31) = v469;
                    if (!v24)
                    {
                        if (!(*((int *)v30) != 1 || *((int *)(v16 + 8)) != 1))
                        {
                            (&g_20021f70)[v462] = *((int *)(537009864 + v462 * 4));
                            if (*((int *)0x2002208c))
                            {
LABEL_8029925:
                                v472 = *((int *)(v16 + 12));
                                v473 = g_20021cbc;
                                goto LABEL_802992d;
                            }
                        }
                        else if (!(!*((int *)0x2002208c)))
                        {
                            goto LABEL_8029925;
                        }
                    }
                    else
                    {
                        if (*((int *)0x2002208c))
                        {
                            v472 = *((int *)(v16 + 12));
                            v473 = g_20021cbc;
                            if (v24 == 1 && !*((int *)v30))
                            {
                                if (v472 != 1)
                                    goto LABEL_802a34d;
                                g_20021de4 = v473;
LABEL_802a34d:
                                v480 = *((int *)v62);
                                (&g_20021f70)[v462] = *((int *)(537009864 + v462 * 4));
                                v481 = v462 + 1;
                                v8 = v480;
                                *(v35) = v481;
                                *(v36) = 0;
                                v482 = (v481 == v8 ? 0 : v481);
                                v483 = *(v38);
                                *(v35) = v482;
                                v477 = v483 + 1;
                                (&g_20021e64)[v482] = 3352888192;
                                *(v38) = v477;
LABEL_802995f:
                                if (v477 >= v50)
                                    *(v38) = 0;
                                v484 = *((int *)(537010524 + v473 * 4));
                                v485 = (int)(*((int *)(v12 + v473 * 4)) * g_20022154 + v427) + *((int *)(537010524 + (v473 - 1) * 4));
                                *((int *)v59) = v485;
                                v475 = (v484 <= v485 ? v484 - 1 : v485);
                                *((int *)v59) = v475;
                                goto LABEL_80299af;
                            }
LABEL_802992d:
                            if (v472 != 1 || g_20021de4 == v473)
                            {
                                v476 = *(v36);
                                v8 = *((int *)v62);
                                v471 = v471 & 0xfffffff | ((((CmpF(v476, v469) >> 5 & 3 | CmpF(v476, v469) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v476, v469) >> 5 & 3 | CmpF(v476, v469) & 1) & (CmpF(v476, v469) >> 5 & 3 | CmpF(v476, v469) & 1) >> 1 & 1)) * 0x10000000;
                                v477 = *(v38);
                                if (!((v471 & 0xf0000000) >> 31 & 1 ^ v471 >> 28 & 1) && (v24 == 1 || *((int *)(v16 + 8)) == 2))
                                {
                                    *(v36) = 0;
                                    v478 = v462 + 1;
                                    v479 = (v478 == v8 ? 0 : v478);
                                    *(v35) = v479;
                                    (&g_20021e64)[v479] = 3352888192;
                                    v477 += 1;
                                    *(v38) = v477;
                                    goto LABEL_802995f;
                                }
                            }
                        }
                    }
                    v474 = *(v38);
                    *(v36) = v469;
                    *((unsigned int *)v30) = 0;
                    if (v474 >= v50)
                        *(v38) = 0;
                    v8 = *((int *)v62);
                    v475 = 0;
                    *((unsigned int *)v59) = 0;
LABEL_80299af:
                    *((unsigned int *)v30) = v24;
                    if (v8 > 0)
                    {
                        v486 = *(v64);
                        v65 = &g_20021de8;
                        v487 = 0;
                        v488 = *(v21);
                        v469 = 1058642330;
                        v46 = &g_20022070;
                        v26 = *((int *)0x200220fc);
                        v66 = &g_2000104c;
                        v489 = v471 & 0xfffffff | ((((CmpF(v421, 0) >> 5 & 3 | CmpF(v421, 0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v421, 0) >> 5 & 3 | CmpF(v421, 0) & 1) & (CmpF(v421, 0) >> 5 & 3 | CmpF(v421, 0) & 1) >> 1 & 1)) * 0x10000000;
                        v5 = *((int *)0x20021ca0);
                        v29 = *((int *)536875664);
                        v4 = g_20021cbc;
                        v471 = v489 & 0xfffffff | ((((CmpF(v488, 1058642330) >> 5 & 3 | CmpF(v488, 1058642330) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v488, 1058642330) >> 5 & 3 | CmpF(v488, 1058642330) & 1) & (CmpF(v488, 1058642330) >> 5 & 3 | CmpF(v488, 1058642330) & 1) >> 1 & 1)) * 0x10000000;
                        v490 = v475 + g_2002214c;
                        v9 = g_2000104c;
                        v34 = g_20021de8;
                        v14 = g_20021314;
                        v27 = *((int *)0x20022100);
                        v13 = g_20022070;
                        v22 = g_20021e14;
                        v491 = (unsigned int)*(v20);
                        v492 = *(v63);
                        v25 = (((v471 & 0xf0000000) >> 30 | (v471 & 0xf0000000) >> 31 ^ (v471 & 0xf0000000) >> 28) & 1 ? 0 : (((v471 & 0xf0000000) >> 30 | (v471 & 0xf0000000) >> 31 ^ (v471 & 0xf0000000) >> 28) & 1 ^ 1 ? 1 : v490));
                        v2 = 0x20021ee4;
                        v37 = v486 * 0x43fa0000;
                        v493 = (v488 - 1058642330) * *((int *)0x20022098) * 0x411ffbe7;
                        v23 = v4 - 1;
                        v494 = &g_20021e64;
                        v495 = 0x20021f28;
                        v496 = 537009864;
                        lr = 537010200;
                        node = 0x20021f94;
                        v11 = &g_20021fd8[0];
                        v10 = &g_20021ff8[0];
                        v39 = -(v486);
                        v497 = *((int *)0x20022098) * 0.5;
                        v498 = 0;
                        v499 = 0;
                        j = 0;
                        v7 = *(v17);
                        v501 = &g_20021f70;
                        v0 = 0x20021f08;
                        do
                        {
                            v502 = *((int *)v494);
                            v494 += 4;
                            v503 = v471 & 0xfffffff | ((((CmpF(v502, 3352888192) >> 5 & 3 | CmpF(v502, 3352888192) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v502, 3352888192) >> 5 & 3 | CmpF(v502, 3352888192) & 1) & (CmpF(v502, 3352888192) >> 5 & 3 | CmpF(v502, 3352888192) & 1) >> 1 & 1)) * 0x10000000;
                            if ((v503 & 0xf0000000) >> 30 & 1)
                            {
                                *((unsigned int *)v496) = 0;
                                *(v0) = 0;
                                v504 = j * 4;
                                *((unsigned int *)(v494 - 4)) = 0;
                                *((unsigned int *)((char *)&g_20021ea8[0] + v504)) = 0;
                                *((unsigned int *)lr) = 3;
                                v505 = *((int *)(537010524 + v4 * 4));
                                v506 = *((int *)(537010524 + v23 * 4));
                                if (v22 == 1)
                                    v491 = v505 - v506;
                                *((unsigned int *)(v10 + j * 4)) = v506;
                                v507 = v503 & 0xfffffff | ((((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) & (CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000;
                                v508 = (unsigned int)(float)(int)(v490 + v491 * ((v489 & 0xf0000000) >> 31 & 1 ? 1.0 : (((v489 & 0xf0000000) >> 31 ^ 1) & 1 ? 0 : 0x463b8000)));
                                *((unsigned int *)(v11 + j * 4)) = v505;
                                *(node) = (((v507 & 0xf0000000) >> 30 ^ 1) & 1 ? 0 : ((v507 & 0xf0000000) >> 30 & 1 ? 1 : 3));
                                v509 = (int)v508;
                                *(v495) = v508;
                                *((float *)v501) = (float)(int)(v491 - v492);
                                (&g_20021fb8)[j] = v4;
                                *((unsigned int *)(v6 + v504)) = v491;
                                if (v506 > v509)
                                {
                                    v510 = (unsigned int)(float)(int)((int)v508 + *((int *)(v12 + v4 * 4)));
                                    v509 = (int)v510;
                                    *(v495) = v510;
                                }
                                if (v509 > v505)
                                {
                                    v511 = (unsigned int)(float)(int)((int)v509 - *((int *)(v12 + v4 * 4)));
                                    v509 = (int)v511;
                                    *(v495) = v511;
                                }
                                v512 = 907633515 + 196314165 * v7;
                                v513 = v512;
                                v514 = 907633515 + 196314165 * v512;
                                v515 = v507 & 0xfffffff | ((((CmpF(v488 - 0.5, v513 * v497) >> 5 & 3 | CmpF(v488 - 0.5, v513 * v497) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v488 - 0.5, v513 * v497) >> 5 & 3 | CmpF(v488 - 0.5, v513 * v497) & 1) & (CmpF(v488 - 0.5, v513 * v497) >> 5 & 3 | CmpF(v488 - 0.5, v513 * v497) & 1) >> 1 & 1)) * 0x10000000;
                                v7 = v514;
                                v516 = v514;
                                if (!((v515 & 0xf0000000) >> 31 & 1 ^ v515 >> 28 & 1))
                                {
                                    v517 = 907633515 + 196314165 * v514;
                                    v518 = *((int *)537010328) * v516;
                                    v516 = v517;
                                    v7 = v517;
                                }
                                else
                                {
                                    v518 = 0;
                                }
                                *((unsigned int *)(v504 + (char *)&g_20021e88[0])) = v518;
                                v2->field_4 = v25 * v493 * v516;
                                v519 = v5 & v509;
                                v3 = 3;
                                v9 = 0;
                                goto LABEL_8029cc3;
                            }
                            else
                            {
                                v509 = *(v495);
                                v505 = *((int *)(v11 + j * 4));
                                v506 = *((int *)(v10 + j * 4));
                                v515 = v503 & 0xfffffff | ((((CmpF(v502, 0x3ff33333) >> 5 & 3 | CmpF(v502, 0x3ff33333) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v502, 0x3ff33333) >> 5 & 3 | CmpF(v502, 0x3ff33333) & 1) & (CmpF(v502, 0x3ff33333) >> 5 & 3 | CmpF(v502, 0x3ff33333) & 1) >> 1 & 1)) * 0x10000000;
                                v519 = v5 & v509;
                                if (!((v515 & 0xf0000000) >> 30 & 1 | (v515 & 0xf0000000) >> 31 & 1 ^ (v515 & 0xf0000000) >> 28 & 1))
                                {
                                    *((unsigned int *)(v494 - 4)) = 0;
                                    *((unsigned int *)lr) = 0;
                                    v3 = 0;
LABEL_8029cc3:
                                    v502 = 0;
                                    goto LABEL_8029cc7;
                                }
                                else
                                {
                                    v515 = v515 & 0xfffffff | ((((CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) & (CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                    v3 = *((int *)lr);
                                    if ((v515 & 0xf0000000) >> 30 & 1 | (v515 & 0xf0000000) >> 31 & 1 ^ (v515 & 0xf0000000) >> 28 & 1 || *((int *)(537010524 + v4 * 4)) > v14 && v29)
                                    {
LABEL_8029cc7:
                                        if (!v3)
                                            goto LABEL_8029f8d;
                                        goto LABEL_8029ccf;
                                    }
                                    else
                                    {
                                        v520 = v519 + 1 & v5;
                                        v521 = *((short *)(v27 + v519 * 2));
                                        v522 = *((short *)(v26 + v519 * 2));
                                        v523 = v502 + v502;
                                        v524 = g_20021e88[j];
                                        v525 = v523 * (v521 + (*((short *)(v27 + v520 * 2)) - v521) * *(v0));
                                        v526 = 1.0 - v524;
                                        v469 = v523 * (v522 + (*((short *)(v26 + v520 * 2)) - v522) * *(v0));
                                        v487 += v526 * v525 + v524 * v469;
                                        v498 += v524 * v525 + v526 * v469;
                                        if (v3)
                                        {
LABEL_8029ccf:
                                            v527 = *(v501);
                                            v528 = j * 4;
                                            v529 = v6 + (char *)v528;
                                            v530 = *(v529);
                                            v531 = (int)v527;
                                            v532 = v515 & 0xfffffff | ((((CmpF(v530, v491) >> 5 & 3 | CmpF(v530, v491) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v530, v491) >> 5 & 3 | CmpF(v530, v491) & 1) & (CmpF(v530, v491) >> 5 & 3 | CmpF(v530, v491) & 1) >> 1 & 1)) * 0x10000000;
                                            v469 = v491 - v492;
                                            v533 = v532 & 0xfffffff | ((((CmpF(v531, v469) >> 5 & 3 | CmpF(v531, v469) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((int)v527, v469) >> 5 & 3 | CmpF((int)v527, v469) & 1) & (CmpF(v531, v469) >> 5 & 3 | CmpF((int)v527, v469) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v532 & 0xf0000000) >> 30 & 1 | (v532 & 0xf0000000) >> 31 & 1 ^ (v532 & 0xf0000000) >> 28 & 1)
                                                goto LABEL_0x8029cfb;
                                            *(v529) = v491;
                                            v534 = (unsigned int)(((v533 & 0xf0000000) >> 30 | (v533 & 0xf0000000) >> 31 ^ (v533 & 0xf0000000) >> 28) & 1 ^ 1 ? (float)(int)v469 : v527);
                                            v535 = v533 & 0xfffffff | ((((CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) & (CmpF(v502, 0.0) >> 5 & 3 | CmpF(v502, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v533 & 0xf0000000) >> 30 & 1 | (v533 & 0xf0000000) >> 31 & 1 ^ (v533 & 0xf0000000) >> 28 & 1)
                                                goto LABEL_0x8029d0f;
                                            *(v501) = v534;
                                            v536 = *((int *)v496);
                                            if ((v535 & 0xf0000000) < 0)
                                            {
                                                v3 = 0;
                                                v537 = (float)(v399 - 841731191);
                                            }
                                            else if (v536 > (int)v534)
                                            {
                                                v537 = (float)(v502 - v486);
                                                v399 = v39;
                                                v3 = 1;
                                            }
                                            else
                                            {
                                                v535 = v535 & 0xfffffff | ((((CmpF(v502, 1.0) >> 5 & 3 | CmpF(v502, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v502, 1.0) >> 5 & 3 | CmpF(v502, 1.0) & 1) & (CmpF(v502, 1.0) >> 5 & 3 | CmpF(v502, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                                                if ((v535 & 0xf0000000) >> 31 & 1 ^ v535 >> 28 & 1 || v3 != 3)
                                                {
                                                    v537 = (float)(v486 + v502);
                                                    v399 = v486;
                                                    v3 = 3;
                                                }
                                                else
                                                {
                                                    v399 = 0;
                                                    v537 = (float)1.0;
                                                    v3 = 2;
                                                }
                                            }
                                            v538 = v535 & 0xfffffff | ((((CmpF(v491, 0x447a0000) >> 5 & 3 | CmpF(v491, 0x447a0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v491, 0x447a0000) >> 5 & 3 | CmpF(v491, 0x447a0000) & 1) & (CmpF(v491, 0x447a0000) >> 5 & 3 | CmpF(v491, 0x447a0000) & 1) >> 1 & 1)) * 0x10000000;
                                            if ((v538 & 0xf0000000) >> 31 & 1)
                                            {
                                                if (v9)
                                                    goto LABEL_8029fe9;
                                            }
                                            else
                                            {
                                                if (v536 + 749 < (int)v534 || v9 || v3 != 2 || (v538 = v538 & 0xfffffff | ((((CmpF((unsigned long long)v492, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v492, 0x437a0000) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v492, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v492, 0x437a0000) & 1) & (CmpF((unsigned long long)v492, 0x437a0000) >> 5 & 3 | CmpF((unsigned long long)v492, 0x437a0000) & 1) >> 1 & 1)) * 0x10000000, !((v538 & 0xf0000000) >> 30 & 1) || (v538 = v538 & 0xfffffff | ((((CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) & (CmpF((unsigned long long)v155, 1035256401) >> 5 & 3 | CmpF((unsigned long long)v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000, (v538 & 0xf0000000) >> 30 & 1 || *(node))))
                                                {
LABEL_8029fe9:
                                                    v538 = v538 & 0xfffffff | ((((CmpF(v486 * 0x447a0000, (unsigned int)v537) >> 5 & 3 | CmpF(v486 * 0x447a0000, (unsigned int)v537) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v486 * 0x447a0000, (unsigned int)v537) >> 5 & 3 | CmpF(v486 * 0x447a0000, (unsigned int)v537) & 1) & (CmpF(v486 * 0x447a0000, (unsigned int)v537) >> 5 & 3 | CmpF(v486 * 0x447a0000, (unsigned int)v537) & 1) >> 1 & 1)) * 0x10000000;
                                                    if (!((v538 & 0xf0000000) >> 30 & 1 | (v538 & 0xf0000000) >> 31 & 1 ^ (v538 & 0xf0000000) >> 28 & 1))
                                                    {
                                                        v538 = v538 & 0xfffffff | ((((CmpF(v37, (unsigned int)v537) >> 5 & 3 | CmpF(v37, (unsigned int)v537) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v37, (unsigned int)v537) >> 5 & 3 | CmpF(v37, (unsigned int)v537) & 1) & (CmpF(v37, (unsigned int)v537) >> 5 & 3 | CmpF(v37, (unsigned int)v537) & 1) >> 1 & 1)) * 0x10000000;
                                                        if ((v538 & 0xf0000000) >> 31 & 1 && !v9 && v3 == 1)
                                                        {
                                                            v538 = v538 & 0xfffffff | ((((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) & (CmpF(v155, 1035256401) >> 5 & 3 | CmpF(v155, 1035256401) & 1) >> 1 & 1)) * 0x10000000;
                                                            if (!((v538 & 0xf0000000) >> 30 & 1))
                                                                v34 = (!*(node) ? 4 : v34);
                                                        }
                                                    }
LABEL_8029d49:
                                                    if (v506 >= v519 - 63)
                                                        v537 = (float)(v537 * (v519 - v506) * 0x3c800000);
                                                    if (v519 + 63 >= v505)
                                                        v537 = (float)(v537 * (v505 - v519) * 0x3c800000);
                                                    *((unsigned int *)lr) = v3;
                                                    v539 = v528 + &g_20021ea8[0];
                                                    v540 = *(v539);
                                                    *((float *)(v494 - 4)) = v537;
                                                    v541 = v430 + v540;
                                                    v542 = v15 + v536;
                                                    v515 = v538 & 0xfffffff | ((((CmpF(v541, 1.0) >> 5 & 3 | CmpF(v541, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v541, 1.0) >> 5 & 3 | CmpF(v541, 1.0) & 1) & (CmpF(v541, 1.0) >> 5 & 3 | CmpF(v541, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                                                    *((unsigned int *)v496) = v542;
                                                    if (!((v515 & 0xf0000000) >> 31 & 1 ^ v515 >> 28 & 1))
                                                    {
                                                        *(v539) = v541 - 1.0;
                                                        *((unsigned int *)v496) = v542 + 1;
                                                        goto LABEL_8029db5;
                                                    }
                                                    else
                                                    {
                                                        v515 = v515 & 0xfffffff | ((((CmpF(v541, 0.0) >> 5 & 3 | CmpF(v541, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v541, 0.0) >> 5 & 3 | CmpF(v541, 0.0) & 1) & (CmpF(v541, 0.0) >> 5 & 3 | CmpF(v541, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                                        if ((v515 & 0xf0000000) >= 0)
                                                        {
                                                            *(v539) = v541;
                                                            goto LABEL_8029db5;
                                                        }
                                                        else
                                                        {
                                                            *(v539) = v541 + 1.0;
                                                            *((unsigned int *)v496) = v542 - 1;
                                                            goto LABEL_8029db5;
                                                        }
                                                    }
                                                }
                                            }
                                            v34 = 4;
                                            goto LABEL_8029d49;
                                        }
                                        else
                                        {
LABEL_8029f8d:
                                            v509 = *(v495);
                                            v537 = 2989214839;
                                            *((unsigned int *)(v494 - 4)) = 2989214839;
                                        }
                                    }
                                }
                            }
LABEL_8029db5:
                            v2 = &v2->field_4;
                            v543 = v515 & 0xfffffff | ((((CmpF(v499, (unsigned int)v537) >> 5 & 3 | CmpF(v499, (unsigned int)v537) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v499, (unsigned int)v537) >> 5 & 3 | CmpF(v499, (unsigned int)v537) & 1) & (CmpF(v499, (unsigned int)v537) >> 5 & 3 | CmpF(v499, (unsigned int)v537) & 1) >> 1 & 1)) * 0x10000000;
                            v544 = &(&v86)[4 * v2->field_4];
                            v499 = (unsigned int)((v543 & 0xf0000000) >> 31 & 1 ? v537 : v499);
                            v545 = *((int *)((char *)v544 - 16));
                            v546 = *(v0);
                            v547 = *((int *)((char *)v544 - 32));
                            v13 = j;
                            v548 = v546 + v545;
                            v549 = v509 + v547;
                            v471 = v543 & 0xfffffff | ((((CmpF(v548, 1.0) >> 5 & 3 | CmpF(v548, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v548, 1.0) >> 5 & 3 | CmpF(v548, 1.0) & 1) & (CmpF(v548, 1.0) >> 5 & 3 | CmpF(v548, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v495) = v549;
                            if (!((v471 & 0xf0000000) >> 31 & 1 ^ v471 >> 28 & 1))
                            {
                                v550 = v549 + 1;
                                *(v0) = v548 - 1.0;
                            }
                            else
                            {
                                v471 = v471 & 0xfffffff | ((((CmpF(v548, 0.0) >> 5 & 3 | CmpF(v548, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v548, 0.0) >> 5 & 3 | CmpF(v548, 0.0) & 1) & (CmpF(v548, 0.0) >> 5 & 3 | CmpF(v548, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                v550 = ((v471 & 0xf0000000) >> 31 & 1 ? v549 - 1 : v549);
                                *(v0) = ((v471 & 0xf0000000) >> 31 & 1 ? v548 + 1.0 : v548);
                            }
                            if (v505 >= v550)
                                v506 = (v506 <= v550 ? v550 : (v550 < v506 ? v505 : v506));
                            v551 = v495 + 1;
                            *(v495) = v506;
                            j += 1;
                            v0 += 1;
                            v496 += 4;
                            lr = lr + 4;
                            node += 1;
                            v501 += 1;
                            v495 = v551;
                        } while (j != v8);
                        *((unsigned int *)v66) = v9;
                        *((unsigned int *)v65) = v34;
                        *((unsigned int *)v20) = v491;
                        *(v17) = v7;
                        *((unsigned int *)v46) = v13;
                    }
                    else
                    {
                        v487 = 0;
                        v498 = 0;
                    }
                    v552 = v67 + *(v54) * 1065185444;
                    v553 = *((int *)v51);
                    *(v54) = v552;
                    if (1000 > v553)
                    {
                        v554 = v552 * 0x38000100;
                        v555 = v498 * v554;
                        v556 = v487 * v554;
                    }
                    else
                    {
                        v556 = 0;
                        v555 = 0;
                    }
                    v557 = *(v32);
                    v558 = *((int *)v44);
                    v559 = v471 & 0xfffffff | ((((CmpF(v557, 0.0) >> 5 & 3 | CmpF(v557, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v557, 0.0) >> 5 & 3 | CmpF(v557, 0.0) & 1) & (CmpF(v557, 0.0) >> 5 & 3 | CmpF(v557, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                    v560 = *(v33) * 0x3f7fbe77;
                    if ((v559 & 0xf0000000) >> 30 & 1)
                        goto LABEL_0x802a101;
                    v561 = (((v559 & 0xf0000000) >> 30 ^ 1) & 1 ? v560 + 981668463 : v560);
                    v562 = v555 * v561;
                    v563 = v556 * v561;
                    v564 = v559 & 0xfffffff | ((((CmpF(v562, 1.0) >> 5 & 3 | CmpF(v562, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v562, 1.0) >> 5 & 3 | CmpF(v562, 1.0) & 1) & (CmpF(v562, 1.0) >> 5 & 3 | CmpF(v562, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v565 = v564 & 0xfffffff | ((((CmpF(v563, 1.0) >> 5 & 3 | CmpF(v563, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v563, 1.0) >> 5 & 3 | CmpF(v563, 1.0) & 1) & (CmpF(v563, 1.0) >> 5 & 3 | CmpF(v563, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v566 = (v564 & 0xf0000000 & 0x20000000 && !(v564 & 0xf0000000 & 0x40000000) ? 1.0 : v562);
                    v567 = (v565 & 0xf0000000 & 0x20000000 && !(v565 & 0xf0000000 & 0x40000000) ? 1.0 : v563);
                    v568 = v565 & 0xfffffff | ((((CmpF(v566, -1.0) >> 5 & 3 | CmpF(v566, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v566, -1.0) >> 5 & 3 | CmpF(v566, -1.0) & 1) & (CmpF(v566, -1.0) >> 5 & 3 | CmpF(v566, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v569 = v568 & 0xfffffff | ((((CmpF(v567, -1.0) >> 5 & 3 | CmpF(v567, -1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v567, -1.0) >> 5 & 3 | CmpF(v567, -1.0) & 1) & (CmpF(v567, -1.0) >> 5 & 3 | CmpF(v567, -1.0) & 1) >> 1 & 1)) * 0x10000000;
                    v570 = v425 + v18->field_0 * ((((v569 & 0xf0000000) >> 3 ^ v569 & 0xf0000000) & 0x10000000 ? -1.0 : v567) - v425);
                    v571 = v419 + ((((v568 & 0xf0000000) >> 3 ^ v568 & 0xf0000000) & 0x10000000 ? -1.0 : v566) - v419) * v18->field_0;
                    v572 = (!*((int *)(v16 + 36)) ? v570 : v425);
                    v573 = (!*((int *)(v16 + 36)) ? v571 : v419);
                    *(v33) = v561;
                    v574 = v570;
                    if (v558 == g_20021de4)
                    {
                        v469 = -(g_20024130) + g_20024138 * 1065302884;
                        v575 = v573 * 1060320051;
                        v576 = v572 * 1060320051;
                        v573 = -(g_2002412c) + g_20024134 * 1065302884 + v575;
                        v572 = v469 + v576;
                        g_2002412c = v575;
                        g_20024134 = v573;
                        g_20024138 = v572;
                        g_20024130 = v576;
                    }
                    if (*((int *)v43) != 1)
                    {
LABEL_802a1cd:
                        if (*((int *)v40) == 1)
                            goto LABEL_802a55d;
                        goto LABEL_802a1d7;
                    }
                    v577 = *(0x20021ca4);
                    g_200213b4 = 0;
                    if (*(0x20021ca4) > g_2002210c)
                    {
                        *((short *)(*((int *)0x200220fc) + g_2002210c * 2)) = v573 * 0x46fffe00;
                        *((short *)(*((int *)0x20022100) + g_2002210c * 2)) = v572 * 0x46fffe00;
                    }
                    v578 = *((int *)537010316);
                    v579 = g_2002210c + 1;
                    g_2002210c = v579;
                    if (!*((int *)0x20001290))
                    {
                        if (v558 <= *((int *)537010316))
                            goto LABEL_802a78d;
                        if (*((int *)537009316) > v579)
                            goto LABEL_802a519;
                        v0 = g_20021314;
                        g_20021c9c = 0;
                        g_20021320 = 0;
                        *((unsigned int *)v43) = 0;
                        g_20021314 = v579;
                        sub_8026619();
                        *((unsigned int *)v53) = 1;
                        g_20021328 = (*((int *)0x20001290) == 1 ? g_20022074 | 2176 : g_20022074);
                        v577 = *(0x20021ca4);
                        node = *((int *)0x20021c80);
                        v0 = g_20021c6c;
                        v580 = g_20021c78 + 100;
                        g_200220a4 = *((int *)0x20021c68) + 100;
                        g_200220a8 = v0 + 100;
                        g_200220b4 = v580;
                        g_200220ac = *((int *)0x20021c70) + 100;
                        g_200220b0 = g_20021c74 + 100;
                        g_200220b8 = g_20021c7c + 100;
                        v581 = *((int *)0x20021c84) + 100;
                        g_200220bc = node + 100;
                        g_200220c0 = v581;
                        v558 = *((int *)v44);
                        v578 = *(0x2002208c);
                        v579 = g_2002210c;
                        *(v77) = 480;
                    }
                    if (v558 > v578)
                        goto LABEL_802a751;
LABEL_802a78d:
                    if (v579 >= *((int *)(537010524 + v558 * 4)))
                    {
                        v579 = *((int *)(537010524 + (v558 - 1) * 4));
                        g_2002210c = v579;
                    }
LABEL_802a751:
                    if (v579 >= v577)
                    {
                        v582 = *(v75);
                        g_200213b8 = 2000;
                        g_2002133c = 2000;
                        sub_802734d((v578 < v558 ? 1 : (v558 <= v578 ? 0 : v579)), v582);
                    }
LABEL_802a519:
                    if (g_20021320 || *((int *)v43) != 1 || !v76 && g_20021cb0)
                        goto LABEL_802a1cd;
                    v583 = *((int *)v44);
                    sub_802734d((*((int *)537010316) < v583 ? 1 : (v583 <= *((int *)537010316) ? 0 : v583)), *(v75), v75);
                    if (*((int *)v40) != 1)
                    {
LABEL_802a1d7:
                        if (*((int *)v45) == 1 && g_200213b4)
                        {
                            g_200213ac = 1000;
                            sub_8026619(g_200213b4, &g_200213ac);
                            *((unsigned int *)v45) = 0;
                            *((unsigned int *)v40) = 0;
                            g_200213b4 = 0;
                            goto LABEL_802a1e9;
                        }
                    }
                    else
                    {
LABEL_802a55d:
                        g_200213b4 = *((int *)(537009960 + g_20022070 * 4));
                        *((unsigned int *)v40) = 2;
                        *((unsigned int *)v45) = 0;
LABEL_802a1e9:
                        v584 = v19;
                        v585 = *(v584);
                        v586 = v569 & 0xfffffff | ((((CmpF(v574, 0.0) >> 5 & 3 | CmpF(v574, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v574, 0.0) >> 5 & 3 | CmpF(v574, 0.0) & 1) & (CmpF(v574, 0.0) >> 5 & 3 | CmpF(v574, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                        v587 = UnaryOp Abs;
                        v588 = (((v586 & 0xf0000000) >> 31 ^ 1) & 1 ? v574 + v587 : ((v586 & 0xf0000000) >> 31 & 1 ? v587 - v574 : v469));
                        if (!*((int *)(v16 + 20)))
                        {
                            v589 = v586 & 0xfffffff | ((((CmpF(v588, (unsigned int)v585) >> 5 & 3 | CmpF(v588, (unsigned int)v585) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF(v588, (unsigned int)v585) >> 5 & 3 | CmpF(v588, (unsigned int)v585) & 1) & (CmpF(v588, (unsigned int)v585) >> 5 & 3 | CmpF(v588, (unsigned int)v585) & 1) >> 1 & 1)) * 0x10000000;
                            if (!((v589 & 0xf0000000) >> 30 & 1 | (v589 & 0xf0000000) >> 31 & 1 ^ (v589 & 0xf0000000) >> 28 & 1))
                            {
                                v590 = v584;
                                v591 = (float)(v585 * 1063675494 + v588 * 1036831949);
                            }
                            else
                            {
                                v590 = v584;
                                v591 = (float)(v585 * 0x3f7fbe77 + v588 * 981668463);
                            }
                            v191 = v589 & 0xfffffff | ((((CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) & (CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v590) = v591;
                            if ((v191 & 0xf0000000) >> 30 & 1 | (v191 & 0xf0000000) >> 31 & 1 ^ (v191 & 0xf0000000) >> 28 & 1)
                                goto LABEL_802a3d5;
                            goto LABEL_802a23d;
                        }
                        else
                        {
                            v591 = (float)(v585 * 1063675494 + *((int *)(537009864 + *(v35) * 4)) * g_20021e84 * g_2002209c * 1031127695);
                            v191 = v586 & 0xfffffff | ((((CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) & (CmpF((unsigned int)v591, 1.0) >> 5 & 3 | CmpF((unsigned int)v591, 1.0) & 1) >> 1 & 1)) * 0x10000000;
                            *(v19) = v591;
                            if (!((v191 & 0xf0000000) >> 30 & 1 | (v191 & 0xf0000000) >> 31 & 1 ^ (v191 & 0xf0000000) >> 28 & 1))
                            {
LABEL_802a23d:
                                v591 = (float)1.0;
                                *((unsigned int *)v19) = 1.0;
                                goto LABEL_802a247;
                            }
                            else
                            {
LABEL_802a3d5:
                                v191 = v191 & 0xfffffff | ((((CmpF((unsigned int)v591, 0.0) >> 5 & 3 | CmpF((unsigned int)v591, 0.0) & 1) ^ 1) * 0x40000000 - 1 >> 29) + 1 - ((CmpF((unsigned int)v591, 0.0) >> 5 & 3 | CmpF((unsigned int)v591, 0.0) & 1) & (CmpF((unsigned int)v591, 0.0) >> 5 & 3 | CmpF((unsigned int)v591, 0.0) & 1) >> 1 & 1)) * 0x10000000;
                                if ((v191 & 0xf0000000) >> 31 & 1)
                                {
                                    v591 = 0;
                                    *((unsigned int *)v19) = 0;
                                }
LABEL_802a247:
                                v592 = *(v60);
                                v593 = *(v61);
                                *(v60) = v592 + 1 & 63;
                                v594 = a1;
                                *((float *)(v57 + v592 * 4)) = v591;
                                *((short *)(v594 + v41)) = v571 * 0x46fffe00;
                                *((short *)(v594 + v42)) = v574 * 0x46fffe00;
                                *(v61) = v593 + 2;
                                v28 = (unsigned short)v28 + 2;
                                if (v47 <= v28)
                                    break;
                                v400 = v18->field_10;
                                v188 = g_20022154;
                                v398 = v28;
                            }
                        }
                    }
                }
            }
        }
    }
    g_20021c90 = g_e0001004;
    g_20021c94 = g_e0001004;
    g_2002205c = 0;
    return;
}
