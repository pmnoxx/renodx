// Found in Wuchang

#include "../../common.hlsl"

Texture2D<float4> t0 : register(t0);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t2 : register(t2);

cbuffer cb0 : register(b0) {
  float cb0_005x : packoffset(c005.x);
  float cb0_005y : packoffset(c005.y);
  float cb0_005z : packoffset(c005.z);
  float cb0_005w : packoffset(c005.w);
  float cb0_008x : packoffset(c008.x);
  float cb0_008y : packoffset(c008.y);
  float cb0_008z : packoffset(c008.z);
  float cb0_008w : packoffset(c008.w);
  float cb0_009x : packoffset(c009.x);
  float cb0_010x : packoffset(c010.x);
  float cb0_010y : packoffset(c010.y);
  float cb0_010z : packoffset(c010.z);
  float cb0_010w : packoffset(c010.w);
  float cb0_011x : packoffset(c011.x);
  float cb0_011y : packoffset(c011.y);
  float cb0_011z : packoffset(c011.z);
  float cb0_011w : packoffset(c011.w);
  float cb0_012x : packoffset(c012.x);
  float cb0_012y : packoffset(c012.y);
  float cb0_012z : packoffset(c012.z);
  float cb0_013x : packoffset(c013.x);
  float cb0_013y : packoffset(c013.y);
  float cb0_013z : packoffset(c013.z);
  float cb0_013w : packoffset(c013.w);
  float cb0_014x : packoffset(c014.x);
  float cb0_014y : packoffset(c014.y);
  float cb0_014z : packoffset(c014.z);
  float cb0_015x : packoffset(c015.x);
  float cb0_015y : packoffset(c015.y);
  float cb0_015z : packoffset(c015.z);
  float cb0_015w : packoffset(c015.w);
  float cb0_016x : packoffset(c016.x);
  float cb0_016y : packoffset(c016.y);
  float cb0_016z : packoffset(c016.z);
  float cb0_016w : packoffset(c016.w);
  float cb0_017x : packoffset(c017.x);
  float cb0_017y : packoffset(c017.y);
  float cb0_017z : packoffset(c017.z);
  float cb0_017w : packoffset(c017.w);
  float cb0_018x : packoffset(c018.x);
  float cb0_018y : packoffset(c018.y);
  float cb0_018z : packoffset(c018.z);
  float cb0_018w : packoffset(c018.w);
  float cb0_019x : packoffset(c019.x);
  float cb0_019y : packoffset(c019.y);
  float cb0_019z : packoffset(c019.z);
  float cb0_019w : packoffset(c019.w);
  float cb0_020x : packoffset(c020.x);
  float cb0_020y : packoffset(c020.y);
  float cb0_020z : packoffset(c020.z);
  float cb0_020w : packoffset(c020.w);
  float cb0_021x : packoffset(c021.x);
  float cb0_021y : packoffset(c021.y);
  float cb0_021z : packoffset(c021.z);
  float cb0_021w : packoffset(c021.w);
  float cb0_022x : packoffset(c022.x);
  float cb0_022y : packoffset(c022.y);
  float cb0_022z : packoffset(c022.z);
  float cb0_022w : packoffset(c022.w);
  float cb0_023x : packoffset(c023.x);
  float cb0_023y : packoffset(c023.y);
  float cb0_023z : packoffset(c023.z);
  float cb0_023w : packoffset(c023.w);
  float cb0_024x : packoffset(c024.x);
  float cb0_024y : packoffset(c024.y);
  float cb0_024z : packoffset(c024.z);
  float cb0_024w : packoffset(c024.w);
  float cb0_025x : packoffset(c025.x);
  float cb0_025y : packoffset(c025.y);
  float cb0_025z : packoffset(c025.z);
  float cb0_025w : packoffset(c025.w);
  float cb0_026x : packoffset(c026.x);
  float cb0_026y : packoffset(c026.y);
  float cb0_026z : packoffset(c026.z);
  float cb0_026w : packoffset(c026.w);
  float cb0_027x : packoffset(c027.x);
  float cb0_027y : packoffset(c027.y);
  float cb0_027z : packoffset(c027.z);
  float cb0_027w : packoffset(c027.w);
  float cb0_028x : packoffset(c028.x);
  float cb0_028y : packoffset(c028.y);
  float cb0_028z : packoffset(c028.z);
  float cb0_028w : packoffset(c028.w);
  float cb0_029x : packoffset(c029.x);
  float cb0_029y : packoffset(c029.y);
  float cb0_029z : packoffset(c029.z);
  float cb0_029w : packoffset(c029.w);
  float cb0_030x : packoffset(c030.x);
  float cb0_030y : packoffset(c030.y);
  float cb0_030z : packoffset(c030.z);
  float cb0_030w : packoffset(c030.w);
  float cb0_031x : packoffset(c031.x);
  float cb0_031y : packoffset(c031.y);
  float cb0_031z : packoffset(c031.z);
  float cb0_031w : packoffset(c031.w);
  float cb0_032x : packoffset(c032.x);
  float cb0_032y : packoffset(c032.y);
  float cb0_032z : packoffset(c032.z);
  float cb0_032w : packoffset(c032.w);
  float cb0_033x : packoffset(c033.x);
  float cb0_033y : packoffset(c033.y);
  float cb0_033z : packoffset(c033.z);
  float cb0_033w : packoffset(c033.w);
  float cb0_034x : packoffset(c034.x);
  float cb0_034y : packoffset(c034.y);
  float cb0_034z : packoffset(c034.z);
  float cb0_034w : packoffset(c034.w);
  float cb0_035z : packoffset(c035.z);
  float cb0_035w : packoffset(c035.w);
  float cb0_036x : packoffset(c036.x);
  float cb0_036y : packoffset(c036.y);
  float cb0_036z : packoffset(c036.z);
  float cb0_036w : packoffset(c036.w);
  float cb0_037x : packoffset(c037.x);
  float cb0_037y : packoffset(c037.y);
  float cb0_037z : packoffset(c037.z);
  float cb0_037w : packoffset(c037.w);
  float cb0_038x : packoffset(c038.x);
  float cb0_039x : packoffset(c039.x);
  float cb0_039y : packoffset(c039.y);
  float cb0_039z : packoffset(c039.z);
  float cb0_040y : packoffset(c040.y);
  float cb0_040z : packoffset(c040.z);
  int cb0_040w : packoffset(c040.w);
  int cb0_041x : packoffset(c041.x);
};

cbuffer cb1 : register(b1) {
  float4 UniformBufferConstants_WorkingColorSpace_000[4] : packoffset(c000.x);
  float4 UniformBufferConstants_WorkingColorSpace_064[4] : packoffset(c004.x);
  float4 UniformBufferConstants_WorkingColorSpace_128[4] : packoffset(c008.x);
  float4 UniformBufferConstants_WorkingColorSpace_192[4] : packoffset(c012.x);
  float4 UniformBufferConstants_WorkingColorSpace_256[4] : packoffset(c016.x);
  int UniformBufferConstants_WorkingColorSpace_320 : packoffset(c020.x);
};

SamplerState s0 : register(s0);

SamplerState s1 : register(s1);

SamplerState s2 : register(s2);

float4 main(
    noperspective float2 TEXCOORD: TEXCOORD,
    noperspective float4 SV_Position: SV_Position,
    nointerpolation uint SV_RenderTargetArrayIndex: SV_RenderTargetArrayIndex) : SV_Target {
  float4 SV_Target;
  float _14[6];
  float _15[6];
  float _16[6];
  float _17[6];
  float _18 = TEXCOORD.x + -0.015625f;
  float _19 = TEXCOORD.y + -0.015625f;
  float _22 = float((uint)(int)(SV_RenderTargetArrayIndex));
  float _43;
  float _44;
  float _45;
  float _46;
  float _47;
  float _48;
  float _49;
  float _50;
  float _51;
  float _109;
  float _110;
  float _111;
  float _635;
  float _671;
  float _682;
  float _746;
  float _925;
  float _936;
  float _947;
  float _1178;
  float _1179;
  float _1180;
  float _1191;
  float _1202;
  float _1384;
  float _1420;
  float _1431;
  float _1470;
  float _1580;
  float _1654;
  float _1728;
  float _1807;
  float _1808;
  float _1809;
  float _1960;
  float _1996;
  float _2007;
  float _2046;
  float _2156;
  float _2230;
  float _2304;
  float _2383;
  float _2384;
  float _2385;
  float _2562;
  float _2563;
  float _2564;
  if (!(cb0_041x == 1)) {
    if (!(cb0_041x == 2)) {
      if (!(cb0_041x == 3)) {
        bool _32 = (cb0_041x == 4);
        _43 = select(_32, 1.0f, 1.7050515413284302f);
        _44 = select(_32, 0.0f, -0.6217905879020691f);
        _45 = select(_32, 0.0f, -0.0832584798336029f);
        _46 = select(_32, 0.0f, -0.13025718927383423f);
        _47 = select(_32, 1.0f, 1.1408027410507202f);
        _48 = select(_32, 0.0f, -0.010548528283834457f);
        _49 = select(_32, 0.0f, -0.024003278464078903f);
        _50 = select(_32, 0.0f, -0.1289687603712082f);
        _51 = select(_32, 1.0f, 1.152971863746643f);
      } else {
        _43 = 0.6954522132873535f;
        _44 = 0.14067870378494263f;
        _45 = 0.16386906802654266f;
        _46 = 0.044794563204050064f;
        _47 = 0.8596711158752441f;
        _48 = 0.0955343171954155f;
        _49 = -0.005525882821530104f;
        _50 = 0.004025210160762072f;
        _51 = 1.0015007257461548f;
      }
    } else {
      _43 = 1.02579927444458f;
      _44 = -0.020052503794431686f;
      _45 = -0.0057713985443115234f;
      _46 = -0.0022350111976265907f;
      _47 = 1.0045825242996216f;
      _48 = -0.002352306619286537f;
      _49 = -0.005014004185795784f;
      _50 = -0.025293385609984398f;
      _51 = 1.0304402112960815f;
    }
  } else {
    _43 = 1.379158854484558f;
    _44 = -0.3088507056236267f;
    _45 = -0.07034677267074585f;
    _46 = -0.06933528929948807f;
    _47 = 1.0822921991348267f;
    _48 = -0.012962047010660172f;
    _49 = -0.002159259282052517f;
    _50 = -0.045465391129255295f;
    _51 = 1.0477596521377563f;
  }
  if ((uint)cb0_040w > (uint)2) {
    float _62 = exp2(log2(_18 * 1.0322580337524414f) * 0.012683313339948654f);
    float _63 = exp2(log2(_19 * 1.0322580337524414f) * 0.012683313339948654f);
    float _64 = exp2(log2(_22 * 0.032258063554763794f) * 0.012683313339948654f);
    _109 = (exp2(log2(max(0.0f, (_62 + -0.8359375f)) / (18.8515625f - (_62 * 18.6875f))) * 6.277394771575928f) * 100.0f);
    _110 = (exp2(log2(max(0.0f, (_63 + -0.8359375f)) / (18.8515625f - (_63 * 18.6875f))) * 6.277394771575928f) * 100.0f);
    _111 = (exp2(log2(max(0.0f, (_64 + -0.8359375f)) / (18.8515625f - (_64 * 18.6875f))) * 6.277394771575928f) * 100.0f);
  } else {
    _109 = ((exp2((_18 * 14.45161247253418f) + -6.07624626159668f) * 0.18000000715255737f) + -0.002667719265446067f);
    _110 = ((exp2((_19 * 14.45161247253418f) + -6.07624626159668f) * 0.18000000715255737f) + -0.002667719265446067f);
    _111 = ((exp2((_22 * 0.4516128897666931f) + -6.07624626159668f) * 0.18000000715255737f) + -0.002667719265446067f);
  }
  float _126 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _111, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _110, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _109)));
  float _129 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _111, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _110, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _109)));
  float _132 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _111, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _110, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _109)));
  float _133 = dot(float3(_126, _129, _132), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));

  SetUngradedAP1(float3(_126, _129, _132));

  float _137 = (_126 / _133) + -1.0f;
  float _138 = (_129 / _133) + -1.0f;
  float _139 = (_132 / _133) + -1.0f;
  float _151 = (1.0f - exp2(((_133 * _133) * -4.0f) * cb0_036z)) * (1.0f - exp2(dot(float3(_137, _138, _139), float3(_137, _138, _139)) * -4.0f));
  float _167 = ((mad(-0.06368283927440643f, _132, mad(-0.32929131388664246f, _129, (_126 * 1.370412826538086f))) - _126) * _151) + _126;
  float _168 = ((mad(-0.010861567221581936f, _132, mad(1.0970908403396606f, _129, (_126 * -0.08343426138162613f))) - _129) * _151) + _129;
  float _169 = ((mad(1.203694462776184f, _132, mad(-0.09862564504146576f, _129, (_126 * -0.02579325996339321f))) - _132) * _151) + _132;
  float _170 = dot(float3(_167, _168, _169), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));
  float _184 = cb0_019w + cb0_024w;
  float _198 = cb0_018w * cb0_023w;
  float _212 = cb0_017w * cb0_022w;
  float _226 = cb0_016w * cb0_021w;
  float _240 = cb0_015w * cb0_020w;
  float _244 = _167 - _170;
  float _245 = _168 - _170;
  float _246 = _169 - _170;
  float _304 = saturate(_170 / cb0_035z);
  float _308 = (_304 * _304) * (3.0f - (_304 * 2.0f));
  float _309 = 1.0f - _308;
  float _318 = cb0_019w + cb0_034w;
  float _327 = cb0_018w * cb0_033w;
  float _336 = cb0_017w * cb0_032w;
  float _345 = cb0_016w * cb0_031w;
  float _354 = cb0_015w * cb0_030w;
  float _417 = saturate((_170 - cb0_035w) / (cb0_036x - cb0_035w));
  float _421 = (_417 * _417) * (3.0f - (_417 * 2.0f));
  float _430 = cb0_019w + cb0_029w;
  float _439 = cb0_018w * cb0_028w;
  float _448 = cb0_017w * cb0_027w;
  float _457 = cb0_016w * cb0_026w;
  float _466 = cb0_015w * cb0_025w;
  float _524 = _308 - _421;
  float _535 = ((_421 * (((cb0_019x + cb0_034x) + _318) + (((cb0_018x * cb0_033x) * _327) * exp2(log2(exp2(((cb0_016x * cb0_031x) * _345) * log2(max(0.0f, ((((cb0_015x * cb0_030x) * _354) * _244) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017x * cb0_032x) * _336)))))) + (_309 * (((cb0_019x + cb0_024x) + _184) + (((cb0_018x * cb0_023x) * _198) * exp2(log2(exp2(((cb0_016x * cb0_021x) * _226) * log2(max(0.0f, ((((cb0_015x * cb0_020x) * _240) * _244) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017x * cb0_022x) * _212))))))) + ((((cb0_019x + cb0_029x) + _430) + (((cb0_018x * cb0_028x) * _439) * exp2(log2(exp2(((cb0_016x * cb0_026x) * _457) * log2(max(0.0f, ((((cb0_015x * cb0_025x) * _466) * _244) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017x * cb0_027x) * _448))))) * _524);
  float _537 = ((_421 * (((cb0_019y + cb0_034y) + _318) + (((cb0_018y * cb0_033y) * _327) * exp2(log2(exp2(((cb0_016y * cb0_031y) * _345) * log2(max(0.0f, ((((cb0_015y * cb0_030y) * _354) * _245) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017y * cb0_032y) * _336)))))) + (_309 * (((cb0_019y + cb0_024y) + _184) + (((cb0_018y * cb0_023y) * _198) * exp2(log2(exp2(((cb0_016y * cb0_021y) * _226) * log2(max(0.0f, ((((cb0_015y * cb0_020y) * _240) * _245) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017y * cb0_022y) * _212))))))) + ((((cb0_019y + cb0_029y) + _430) + (((cb0_018y * cb0_028y) * _439) * exp2(log2(exp2(((cb0_016y * cb0_026y) * _457) * log2(max(0.0f, ((((cb0_015y * cb0_025y) * _466) * _245) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017y * cb0_027y) * _448))))) * _524);
  float _539 = ((_421 * (((cb0_019z + cb0_034z) + _318) + (((cb0_018z * cb0_033z) * _327) * exp2(log2(exp2(((cb0_016z * cb0_031z) * _345) * log2(max(0.0f, ((((cb0_015z * cb0_030z) * _354) * _246) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017z * cb0_032z) * _336)))))) + (_309 * (((cb0_019z + cb0_024z) + _184) + (((cb0_018z * cb0_023z) * _198) * exp2(log2(exp2(((cb0_016z * cb0_021z) * _226) * log2(max(0.0f, ((((cb0_015z * cb0_020z) * _240) * _246) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017z * cb0_022z) * _212))))))) + ((((cb0_019z + cb0_029z) + _430) + (((cb0_018z * cb0_028z) * _439) * exp2(log2(exp2(((cb0_016z * cb0_026z) * _457) * log2(max(0.0f, ((((cb0_015z * cb0_025z) * _466) * _246) + _170)) * 5.55555534362793f)) * 0.18000000715255737f) * (1.0f / ((cb0_017z * cb0_027z) * _448))))) * _524);

  SetUntonemappedAP1(float3(_535, _537, _539));

  float _575 = ((mad(0.061360642313957214f, _539, mad(-4.540197551250458e-09f, _537, (_535 * 0.9386394023895264f))) - _535) * cb0_036y) + _535;
  float _576 = ((mad(0.169205904006958f, _539, mad(0.8307942152023315f, _537, (_535 * 6.775371730327606e-08f))) - _537) * cb0_036y) + _537;
  float _577 = (mad(-2.3283064365386963e-10f, _537, (_535 * -9.313225746154785e-10f)) * cb0_036y) + _539;
  float _580 = mad(0.16386905312538147f, _577, mad(0.14067868888378143f, _576, (_575 * 0.6954522132873535f)));
  float _583 = mad(0.0955343246459961f, _577, mad(0.8596711158752441f, _576, (_575 * 0.044794581830501556f)));
  float _586 = mad(1.0015007257461548f, _577, mad(0.004025210160762072f, _576, (_575 * -0.005525882821530104f)));
  float _590 = max(max(_580, _583), _586);
  float _595 = (max(_590, 1.000000013351432e-10f) - max(min(min(_580, _583), _586), 1.000000013351432e-10f)) / max(_590, 0.009999999776482582f);
  float _608 = ((_583 + _580) + _586) + (sqrt((((_586 - _583) * _586) + ((_583 - _580) * _583)) + ((_580 - _586) * _580)) * 1.75f);
  float _609 = _608 * 0.3333333432674408f;
  float _610 = _595 + -0.4000000059604645f;
  float _611 = _610 * 5.0f;
  float _615 = max((1.0f - abs(_610 * 2.5f)), 0.0f);
  float _626 = ((float((int)(((int)(uint)((bool)(_611 > 0.0f))) - ((int)(uint)((bool)(_611 < 0.0f))))) * (1.0f - (_615 * _615))) + 1.0f) * 0.02500000037252903f;
  if (!(_609 <= 0.0533333346247673f)) {
    if (!(_609 >= 0.1599999964237213f)) {
      _635 = (((0.23999999463558197f / _608) + -0.5f) * _626);
    } else {
      _635 = 0.0f;
    }
  } else {
    _635 = _626;
  }
  float _636 = _635 + 1.0f;
  float _637 = _636 * _580;
  float _638 = _636 * _583;
  float _639 = _636 * _586;
  if (!((bool)(_637 == _638) && (bool)(_638 == _639))) {
    float _646 = ((_637 * 2.0f) - _638) - _639;
    float _649 = ((_583 - _586) * 1.7320507764816284f) * _636;
    float _651 = atan(_649 / _646);
    bool _654 = (_646 < 0.0f);
    bool _655 = (_646 == 0.0f);
    bool _656 = (_649 >= 0.0f);
    bool _657 = (_649 < 0.0f);
    float _666 = select((_656 && _655), 90.0f, select((_657 && _655), -90.0f, (select((_657 && _654), (_651 + -3.1415927410125732f), select((_656 && _654), (_651 + 3.1415927410125732f), _651)) * 57.2957763671875f)));
    if (_666 < 0.0f) {
      _671 = (_666 + 360.0f);
    } else {
      _671 = _666;
    }
  } else {
    _671 = 0.0f;
  }
  float _673 = min(max(_671, 0.0f), 360.0f);
  if (_673 < -180.0f) {
    _682 = (_673 + 360.0f);
  } else {
    if (_673 > 180.0f) {
      _682 = (_673 + -360.0f);
    } else {
      _682 = _673;
    }
  }
  float _686 = saturate(1.0f - abs(_682 * 0.014814814552664757f));
  float _690 = (_686 * _686) * (3.0f - (_686 * 2.0f));
  float _696 = ((_690 * _690) * ((_595 * 0.18000000715255737f) * (0.029999999329447746f - _637))) + _637;
  float _706 = max(0.0f, mad(-0.21492856740951538f, _639, mad(-0.2365107536315918f, _638, (_696 * 1.4514392614364624f))));
  float _707 = max(0.0f, mad(-0.09967592358589172f, _639, mad(1.17622971534729f, _638, (_696 * -0.07655377686023712f))));
  float _708 = max(0.0f, mad(0.9977163076400757f, _639, mad(-0.006032449658960104f, _638, (_696 * 0.008316148072481155f))));
  float _709 = dot(float3(_706, _707, _708), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));
  float _723 = (cb0_037w + 1.0f) - cb0_037y;
  float _726 = cb0_038x + 1.0f;
  float _728 = _726 - cb0_037z;
  if (cb0_037y > 0.800000011920929f) {
    _746 = (((0.8199999928474426f - cb0_037y) / cb0_037x) + -0.7447274923324585f);
  } else {
    float _737 = (cb0_037w + 0.18000000715255737f) / _723;
    _746 = (-0.7447274923324585f - ((log2(_737 / (2.0f - _737)) * 0.3465735912322998f) * (_723 / cb0_037x)));
  }
  float _749 = ((1.0f - cb0_037y) / cb0_037x) - _746;
  float _751 = (cb0_037z / cb0_037x) - _749;
  float _755 = log2(lerp(_709, _706, 0.9599999785423279f)) * 0.3010300099849701f;
  float _756 = log2(lerp(_709, _707, 0.9599999785423279f)) * 0.3010300099849701f;
  float _757 = log2(lerp(_709, _708, 0.9599999785423279f)) * 0.3010300099849701f;
  float _761 = cb0_037x * (_755 + _749);
  float _762 = cb0_037x * (_756 + _749);
  float _763 = cb0_037x * (_757 + _749);
  float _764 = _723 * 2.0f;
  float _766 = (cb0_037x * -2.0f) / _723;
  float _767 = _755 - _746;
  float _768 = _756 - _746;
  float _769 = _757 - _746;
  float _788 = _728 * 2.0f;
  float _790 = (cb0_037x * 2.0f) / _728;
  float _815 = select((_755 < _746), ((_764 / (exp2((_767 * 1.4426950216293335f) * _766) + 1.0f)) - cb0_037w), _761);
  float _816 = select((_756 < _746), ((_764 / (exp2((_768 * 1.4426950216293335f) * _766) + 1.0f)) - cb0_037w), _762);
  float _817 = select((_757 < _746), ((_764 / (exp2((_769 * 1.4426950216293335f) * _766) + 1.0f)) - cb0_037w), _763);
  float _824 = _751 - _746;
  float _828 = saturate(_767 / _824);
  float _829 = saturate(_768 / _824);
  float _830 = saturate(_769 / _824);
  bool _831 = (_751 < _746);
  float _835 = select(_831, (1.0f - _828), _828);
  float _836 = select(_831, (1.0f - _829), _829);
  float _837 = select(_831, (1.0f - _830), _830);
  float _856 = (((_835 * _835) * (select((_755 > _751), (_726 - (_788 / (exp2(((_755 - _751) * 1.4426950216293335f) * _790) + 1.0f))), _761) - _815)) * (3.0f - (_835 * 2.0f))) + _815;
  float _857 = (((_836 * _836) * (select((_756 > _751), (_726 - (_788 / (exp2(((_756 - _751) * 1.4426950216293335f) * _790) + 1.0f))), _762) - _816)) * (3.0f - (_836 * 2.0f))) + _816;
  float _858 = (((_837 * _837) * (select((_757 > _751), (_726 - (_788 / (exp2(((_757 - _751) * 1.4426950216293335f) * _790) + 1.0f))), _763) - _817)) * (3.0f - (_837 * 2.0f))) + _817;
  float _859 = dot(float3(_856, _857, _858), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));
  float _879 = (cb0_036w * (max(0.0f, (lerp(_859, _856, 0.9300000071525574f))) - _575)) + _575;
  float _880 = (cb0_036w * (max(0.0f, (lerp(_859, _857, 0.9300000071525574f))) - _576)) + _576;
  float _881 = (cb0_036w * (max(0.0f, (lerp(_859, _858, 0.9300000071525574f))) - _577)) + _577;
  float _897 = ((mad(-0.06537103652954102f, _881, mad(1.451815478503704e-06f, _880, (_879 * 1.065374732017517f))) - _879) * cb0_036y) + _879;
  float _898 = ((mad(-0.20366770029067993f, _881, mad(1.2036634683609009f, _880, (_879 * -2.57161445915699e-07f))) - _880) * cb0_036y) + _880;
  float _899 = ((mad(0.9999996423721313f, _881, mad(2.0954757928848267e-08f, _880, (_879 * 1.862645149230957e-08f))) - _881) * cb0_036y) + _881;

  SetTonemappedAP1(_897, _898, _899);

  float _912 = saturate(max(0.0f, mad((UniformBufferConstants_WorkingColorSpace_192[0].z), _899, mad((UniformBufferConstants_WorkingColorSpace_192[0].y), _898, ((UniformBufferConstants_WorkingColorSpace_192[0].x) * _897)))));
  float _913 = saturate(max(0.0f, mad((UniformBufferConstants_WorkingColorSpace_192[1].z), _899, mad((UniformBufferConstants_WorkingColorSpace_192[1].y), _898, ((UniformBufferConstants_WorkingColorSpace_192[1].x) * _897)))));
  float _914 = saturate(max(0.0f, mad((UniformBufferConstants_WorkingColorSpace_192[2].z), _899, mad((UniformBufferConstants_WorkingColorSpace_192[2].y), _898, ((UniformBufferConstants_WorkingColorSpace_192[2].x) * _897)))));
  if (_912 < 0.0031306699384003878f) {
    _925 = (_912 * 12.920000076293945f);
  } else {
    _925 = (((pow(_912, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
  }
  if (_913 < 0.0031306699384003878f) {
    _936 = (_913 * 12.920000076293945f);
  } else {
    _936 = (((pow(_913, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
  }
  if (_914 < 0.0031306699384003878f) {
    _947 = (_914 * 12.920000076293945f);
  } else {
    _947 = (((pow(_914, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
  }
  float _951 = (_936 * 0.9375f) + 0.03125f;
  float _958 = _947 * 15.0f;
  float _959 = floor(_958);
  float _960 = _958 - _959;
  float _962 = (_959 + ((_925 * 0.9375f) + 0.03125f)) * 0.0625f;
  float4 _965 = t0.Sample(s0, float2(_962, _951));
  float _969 = _962 + 0.0625f;
  float4 _972 = t0.Sample(s0, float2(_969, _951));
  float4 _995 = t1.Sample(s1, float2(_962, _951));
  float4 _1001 = t1.Sample(s1, float2(_969, _951));
  float4 _1024 = t2.Sample(s2, float2(_962, _951));
  float4 _1030 = t2.Sample(s2, float2(_969, _951));
  float _1049 = max(6.103519990574569e-05f, (((((lerp(_965.x, _972.x, _960)) * cb0_005y) + (cb0_005x * _925)) + ((lerp(_995.x, _1001.x, _960)) * cb0_005z)) + ((lerp(_1024.x, _1030.x, _960)) * cb0_005w)));
  float _1050 = max(6.103519990574569e-05f, (((((lerp(_965.y, _972.y, _960)) * cb0_005y) + (cb0_005x * _936)) + ((lerp(_995.y, _1001.y, _960)) * cb0_005z)) + ((lerp(_1024.y, _1030.y, _960)) * cb0_005w)));
  float _1051 = max(6.103519990574569e-05f, (((((lerp(_965.z, _972.z, _960)) * cb0_005y) + (cb0_005x * _947)) + ((lerp(_995.z, _1001.z, _960)) * cb0_005z)) + ((lerp(_1024.z, _1030.z, _960)) * cb0_005w)));
  float _1073 = select((_1049 > 0.040449999272823334f), exp2(log2((_1049 * 0.9478672742843628f) + 0.05213269963860512f) * 2.4000000953674316f), (_1049 * 0.07739938050508499f));
  float _1074 = select((_1050 > 0.040449999272823334f), exp2(log2((_1050 * 0.9478672742843628f) + 0.05213269963860512f) * 2.4000000953674316f), (_1050 * 0.07739938050508499f));
  float _1075 = select((_1051 > 0.040449999272823334f), exp2(log2((_1051 * 0.9478672742843628f) + 0.05213269963860512f) * 2.4000000953674316f), (_1051 * 0.07739938050508499f));
  float _1101 = cb0_014x * (((cb0_039y + (cb0_039x * _1073)) * _1073) + cb0_039z);
  float _1102 = cb0_014y * (((cb0_039y + (cb0_039x * _1074)) * _1074) + cb0_039z);
  float _1103 = cb0_014z * (((cb0_039y + (cb0_039x * _1075)) * _1075) + cb0_039z);
  float _1110 = ((cb0_013x - _1101) * cb0_013w) + _1101;
  float _1111 = ((cb0_013y - _1102) * cb0_013w) + _1102;
  float _1112 = ((cb0_013z - _1103) * cb0_013w) + _1103;
  float _1113 = cb0_014x * mad((UniformBufferConstants_WorkingColorSpace_192[0].z), _539, mad((UniformBufferConstants_WorkingColorSpace_192[0].y), _537, (_535 * (UniformBufferConstants_WorkingColorSpace_192[0].x))));
  float _1114 = cb0_014y * mad((UniformBufferConstants_WorkingColorSpace_192[1].z), _539, mad((UniformBufferConstants_WorkingColorSpace_192[1].y), _537, ((UniformBufferConstants_WorkingColorSpace_192[1].x) * _535)));
  float _1115 = cb0_014z * mad((UniformBufferConstants_WorkingColorSpace_192[2].z), _539, mad((UniformBufferConstants_WorkingColorSpace_192[2].y), _537, ((UniformBufferConstants_WorkingColorSpace_192[2].x) * _535)));
  float _1122 = ((cb0_013x - _1113) * cb0_013w) + _1113;
  float _1123 = ((cb0_013y - _1114) * cb0_013w) + _1114;
  float _1124 = ((cb0_013z - _1115) * cb0_013w) + _1115;
  float _1136 = exp2(log2(max(0.0f, _1110)) * cb0_040y);
  float _1137 = exp2(log2(max(0.0f, _1111)) * cb0_040y);
  float _1138 = exp2(log2(max(0.0f, _1112)) * cb0_040y);

  // CustomEdit
  if (RENODX_TONE_MAP_TYPE != 0) {
    return GenerateOutput(float3(_1136, _1137, _1138), cb0_040w);
  }

  [branch]
  if (cb0_040w == 0) {
    do {
      if (UniformBufferConstants_WorkingColorSpace_320 == 0) {
        float _1161 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _1136)));
        float _1164 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _1136)));
        float _1167 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _1136)));
        _1178 = mad(_45, _1167, mad(_44, _1164, (_1161 * _43)));
        _1179 = mad(_48, _1167, mad(_47, _1164, (_1161 * _46)));
        _1180 = mad(_51, _1167, mad(_50, _1164, (_1161 * _49)));
      } else {
        _1178 = _1136;
        _1179 = _1137;
        _1180 = _1138;
      }
      do {
        if (_1178 < 0.0031306699384003878f) {
          _1191 = (_1178 * 12.920000076293945f);
        } else {
          _1191 = (((pow(_1178, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
        }
        do {
          if (_1179 < 0.0031306699384003878f) {
            _1202 = (_1179 * 12.920000076293945f);
          } else {
            _1202 = (((pow(_1179, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
          }
          if (_1180 < 0.0031306699384003878f) {
            _2562 = _1191;
            _2563 = _1202;
            _2564 = (_1180 * 12.920000076293945f);
          } else {
            _2562 = _1191;
            _2563 = _1202;
            _2564 = (((pow(_1180, 0.4166666567325592f)) * 1.0549999475479126f) + -0.054999999701976776f);
          }
        } while (false);
      } while (false);
    } while (false);
  } else {
    if (cb0_040w == 1) {
      float _1229 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _1136)));
      float _1232 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _1136)));
      float _1235 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _1136)));
      float _1245 = max(6.103519990574569e-05f, mad(_45, _1235, mad(_44, _1232, (_1229 * _43))));
      float _1246 = max(6.103519990574569e-05f, mad(_48, _1235, mad(_47, _1232, (_1229 * _46))));
      float _1247 = max(6.103519990574569e-05f, mad(_51, _1235, mad(_50, _1232, (_1229 * _49))));
      _2562 = min((_1245 * 4.5f), ((exp2(log2(max(_1245, 0.017999999225139618f)) * 0.44999998807907104f) * 1.0989999771118164f) + -0.0989999994635582f));
      _2563 = min((_1246 * 4.5f), ((exp2(log2(max(_1246, 0.017999999225139618f)) * 0.44999998807907104f) * 1.0989999771118164f) + -0.0989999994635582f));
      _2564 = min((_1247 * 4.5f), ((exp2(log2(max(_1247, 0.017999999225139618f)) * 0.44999998807907104f) * 1.0989999771118164f) + -0.0989999994635582f));
    } else {
      if ((bool)(cb0_040w == 3) || (bool)(cb0_040w == 5)) {
        _16[0] = cb0_010x;
        _16[1] = cb0_010y;
        _16[2] = cb0_010z;
        _16[3] = cb0_010w;
        _16[4] = cb0_012x;
        _16[5] = cb0_012x;
        _17[0] = cb0_011x;
        _17[1] = cb0_011y;
        _17[2] = cb0_011z;
        _17[3] = cb0_011w;
        _17[4] = cb0_012y;
        _17[5] = cb0_012y;
        float _1324 = cb0_012z * _1122;
        float _1325 = cb0_012z * _1123;
        float _1326 = cb0_012z * _1124;
        float _1329 = mad((UniformBufferConstants_WorkingColorSpace_256[0].z), _1326, mad((UniformBufferConstants_WorkingColorSpace_256[0].y), _1325, ((UniformBufferConstants_WorkingColorSpace_256[0].x) * _1324)));
        float _1332 = mad((UniformBufferConstants_WorkingColorSpace_256[1].z), _1326, mad((UniformBufferConstants_WorkingColorSpace_256[1].y), _1325, ((UniformBufferConstants_WorkingColorSpace_256[1].x) * _1324)));
        float _1335 = mad((UniformBufferConstants_WorkingColorSpace_256[2].z), _1326, mad((UniformBufferConstants_WorkingColorSpace_256[2].y), _1325, ((UniformBufferConstants_WorkingColorSpace_256[2].x) * _1324)));
        float _1339 = max(max(_1329, _1332), _1335);
        float _1344 = (max(_1339, 1.000000013351432e-10f) - max(min(min(_1329, _1332), _1335), 1.000000013351432e-10f)) / max(_1339, 0.009999999776482582f);
        float _1357 = ((_1332 + _1329) + _1335) + (sqrt((((_1335 - _1332) * _1335) + ((_1332 - _1329) * _1332)) + ((_1329 - _1335) * _1329)) * 1.75f);
        float _1358 = _1357 * 0.3333333432674408f;
        float _1359 = _1344 + -0.4000000059604645f;
        float _1360 = _1359 * 5.0f;
        float _1364 = max((1.0f - abs(_1359 * 2.5f)), 0.0f);
        float _1375 = ((float((int)(((int)(uint)((bool)(_1360 > 0.0f))) - ((int)(uint)((bool)(_1360 < 0.0f))))) * (1.0f - (_1364 * _1364))) + 1.0f) * 0.02500000037252903f;
        do {
          if (!(_1358 <= 0.0533333346247673f)) {
            if (!(_1358 >= 0.1599999964237213f)) {
              _1384 = (((0.23999999463558197f / _1357) + -0.5f) * _1375);
            } else {
              _1384 = 0.0f;
            }
          } else {
            _1384 = _1375;
          }
          float _1385 = _1384 + 1.0f;
          float _1386 = _1385 * _1329;
          float _1387 = _1385 * _1332;
          float _1388 = _1385 * _1335;
          do {
            if (!((bool)(_1386 == _1387) && (bool)(_1387 == _1388))) {
              float _1395 = ((_1386 * 2.0f) - _1387) - _1388;
              float _1398 = ((_1332 - _1335) * 1.7320507764816284f) * _1385;
              float _1400 = atan(_1398 / _1395);
              bool _1403 = (_1395 < 0.0f);
              bool _1404 = (_1395 == 0.0f);
              bool _1405 = (_1398 >= 0.0f);
              bool _1406 = (_1398 < 0.0f);
              float _1415 = select((_1405 && _1404), 90.0f, select((_1406 && _1404), -90.0f, (select((_1406 && _1403), (_1400 + -3.1415927410125732f), select((_1405 && _1403), (_1400 + 3.1415927410125732f), _1400)) * 57.2957763671875f)));
              if (_1415 < 0.0f) {
                _1420 = (_1415 + 360.0f);
              } else {
                _1420 = _1415;
              }
            } else {
              _1420 = 0.0f;
            }
            float _1422 = min(max(_1420, 0.0f), 360.0f);
            do {
              if (_1422 < -180.0f) {
                _1431 = (_1422 + 360.0f);
              } else {
                if (_1422 > 180.0f) {
                  _1431 = (_1422 + -360.0f);
                } else {
                  _1431 = _1422;
                }
              }
              do {
                if ((bool)(_1431 > -67.5f) && (bool)(_1431 < 67.5f)) {
                  float _1437 = (_1431 + 67.5f) * 0.029629629105329514f;
                  int _1438 = int(_1437);
                  float _1440 = _1437 - float((int)(_1438));
                  float _1441 = _1440 * _1440;
                  float _1442 = _1441 * _1440;
                  if (_1438 == 3) {
                    _1470 = (((0.1666666716337204f - (_1440 * 0.5f)) + (_1441 * 0.5f)) - (_1442 * 0.1666666716337204f));
                  } else {
                    if (_1438 == 2) {
                      _1470 = ((0.6666666865348816f - _1441) + (_1442 * 0.5f));
                    } else {
                      if (_1438 == 1) {
                        _1470 = (((_1442 * -0.5f) + 0.1666666716337204f) + ((_1441 + _1440) * 0.5f));
                      } else {
                        _1470 = select((_1438 == 0), (_1442 * 0.1666666716337204f), 0.0f);
                      }
                    }
                  }
                } else {
                  _1470 = 0.0f;
                }
                float _1479 = min(max(((((_1344 * 0.27000001072883606f) * (0.029999999329447746f - _1386)) * _1470) + _1386), 0.0f), 65535.0f);
                float _1480 = min(max(_1387, 0.0f), 65535.0f);
                float _1481 = min(max(_1388, 0.0f), 65535.0f);
                float _1494 = min(max(mad(-0.21492856740951538f, _1481, mad(-0.2365107536315918f, _1480, (_1479 * 1.4514392614364624f))), 0.0f), 65504.0f);
                float _1495 = min(max(mad(-0.09967592358589172f, _1481, mad(1.17622971534729f, _1480, (_1479 * -0.07655377686023712f))), 0.0f), 65504.0f);
                float _1496 = min(max(mad(0.9977163076400757f, _1481, mad(-0.006032449658960104f, _1480, (_1479 * 0.008316148072481155f))), 0.0f), 65504.0f);
                float _1497 = dot(float3(_1494, _1495, _1496), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));
                float _1508 = log2(max((lerp(_1497, _1494, 0.9599999785423279f)), 1.000000013351432e-10f));
                float _1509 = _1508 * 0.3010300099849701f;
                float _1510 = log2(cb0_008x);
                float _1511 = _1510 * 0.3010300099849701f;
                do {
                  if (!(!(_1509 <= _1511))) {
                    _1580 = (log2(cb0_008y) * 0.3010300099849701f);
                  } else {
                    float _1518 = log2(cb0_009x);
                    float _1519 = _1518 * 0.3010300099849701f;
                    if ((bool)(_1509 > _1511) && (bool)(_1509 < _1519)) {
                      float _1527 = ((_1508 - _1510) * 0.9030900001525879f) / ((_1518 - _1510) * 0.3010300099849701f);
                      int _1528 = int(_1527);
                      float _1530 = _1527 - float((int)(_1528));
                      float _1532 = _16[_1528];
                      float _1535 = _16[(_1528 + 1)];
                      float _1540 = _1532 * 0.5f;
                      _1580 = dot(float3((_1530 * _1530), _1530, 1.0f), float3(mad((_16[(_1528 + 2)]), 0.5f, mad(_1535, -1.0f, _1540)), (_1535 - _1532), mad(_1535, 0.5f, _1540)));
                    } else {
                      do {
                        if (!(!(_1509 >= _1519))) {
                          float _1549 = log2(cb0_008z);
                          if (_1509 < (_1549 * 0.3010300099849701f)) {
                            float _1557 = ((_1508 - _1518) * 0.9030900001525879f) / ((_1549 - _1518) * 0.3010300099849701f);
                            int _1558 = int(_1557);
                            float _1560 = _1557 - float((int)(_1558));
                            float _1562 = _17[_1558];
                            float _1565 = _17[(_1558 + 1)];
                            float _1570 = _1562 * 0.5f;
                            _1580 = dot(float3((_1560 * _1560), _1560, 1.0f), float3(mad((_17[(_1558 + 2)]), 0.5f, mad(_1565, -1.0f, _1570)), (_1565 - _1562), mad(_1565, 0.5f, _1570)));
                            break;
                          }
                        }
                        _1580 = (log2(cb0_008w) * 0.3010300099849701f);
                      } while (false);
                    }
                  }
                  float _1584 = log2(max((lerp(_1497, _1495, 0.9599999785423279f)), 1.000000013351432e-10f));
                  float _1585 = _1584 * 0.3010300099849701f;
                  do {
                    if (!(!(_1585 <= _1511))) {
                      _1654 = (log2(cb0_008y) * 0.3010300099849701f);
                    } else {
                      float _1592 = log2(cb0_009x);
                      float _1593 = _1592 * 0.3010300099849701f;
                      if ((bool)(_1585 > _1511) && (bool)(_1585 < _1593)) {
                        float _1601 = ((_1584 - _1510) * 0.9030900001525879f) / ((_1592 - _1510) * 0.3010300099849701f);
                        int _1602 = int(_1601);
                        float _1604 = _1601 - float((int)(_1602));
                        float _1606 = _16[_1602];
                        float _1609 = _16[(_1602 + 1)];
                        float _1614 = _1606 * 0.5f;
                        _1654 = dot(float3((_1604 * _1604), _1604, 1.0f), float3(mad((_16[(_1602 + 2)]), 0.5f, mad(_1609, -1.0f, _1614)), (_1609 - _1606), mad(_1609, 0.5f, _1614)));
                      } else {
                        do {
                          if (!(!(_1585 >= _1593))) {
                            float _1623 = log2(cb0_008z);
                            if (_1585 < (_1623 * 0.3010300099849701f)) {
                              float _1631 = ((_1584 - _1592) * 0.9030900001525879f) / ((_1623 - _1592) * 0.3010300099849701f);
                              int _1632 = int(_1631);
                              float _1634 = _1631 - float((int)(_1632));
                              float _1636 = _17[_1632];
                              float _1639 = _17[(_1632 + 1)];
                              float _1644 = _1636 * 0.5f;
                              _1654 = dot(float3((_1634 * _1634), _1634, 1.0f), float3(mad((_17[(_1632 + 2)]), 0.5f, mad(_1639, -1.0f, _1644)), (_1639 - _1636), mad(_1639, 0.5f, _1644)));
                              break;
                            }
                          }
                          _1654 = (log2(cb0_008w) * 0.3010300099849701f);
                        } while (false);
                      }
                    }
                    float _1658 = log2(max((lerp(_1497, _1496, 0.9599999785423279f)), 1.000000013351432e-10f));
                    float _1659 = _1658 * 0.3010300099849701f;
                    do {
                      if (!(!(_1659 <= _1511))) {
                        _1728 = (log2(cb0_008y) * 0.3010300099849701f);
                      } else {
                        float _1666 = log2(cb0_009x);
                        float _1667 = _1666 * 0.3010300099849701f;
                        if ((bool)(_1659 > _1511) && (bool)(_1659 < _1667)) {
                          float _1675 = ((_1658 - _1510) * 0.9030900001525879f) / ((_1666 - _1510) * 0.3010300099849701f);
                          int _1676 = int(_1675);
                          float _1678 = _1675 - float((int)(_1676));
                          float _1680 = _16[_1676];
                          float _1683 = _16[(_1676 + 1)];
                          float _1688 = _1680 * 0.5f;
                          _1728 = dot(float3((_1678 * _1678), _1678, 1.0f), float3(mad((_16[(_1676 + 2)]), 0.5f, mad(_1683, -1.0f, _1688)), (_1683 - _1680), mad(_1683, 0.5f, _1688)));
                        } else {
                          do {
                            if (!(!(_1659 >= _1667))) {
                              float _1697 = log2(cb0_008z);
                              if (_1659 < (_1697 * 0.3010300099849701f)) {
                                float _1705 = ((_1658 - _1666) * 0.9030900001525879f) / ((_1697 - _1666) * 0.3010300099849701f);
                                int _1706 = int(_1705);
                                float _1708 = _1705 - float((int)(_1706));
                                float _1710 = _17[_1706];
                                float _1713 = _17[(_1706 + 1)];
                                float _1718 = _1710 * 0.5f;
                                _1728 = dot(float3((_1708 * _1708), _1708, 1.0f), float3(mad((_17[(_1706 + 2)]), 0.5f, mad(_1713, -1.0f, _1718)), (_1713 - _1710), mad(_1713, 0.5f, _1718)));
                                break;
                              }
                            }
                            _1728 = (log2(cb0_008w) * 0.3010300099849701f);
                          } while (false);
                        }
                      }
                      float _1732 = cb0_008w - cb0_008y;
                      float _1733 = (exp2(_1580 * 3.321928024291992f) - cb0_008y) / _1732;
                      float _1735 = (exp2(_1654 * 3.321928024291992f) - cb0_008y) / _1732;
                      float _1737 = (exp2(_1728 * 3.321928024291992f) - cb0_008y) / _1732;
                      float _1740 = mad(0.15618768334388733f, _1737, mad(0.13400420546531677f, _1735, (_1733 * 0.6624541878700256f)));
                      float _1743 = mad(0.053689517080783844f, _1737, mad(0.6740817427635193f, _1735, (_1733 * 0.2722287178039551f)));
                      float _1746 = mad(1.0103391408920288f, _1737, mad(0.00406073359772563f, _1735, (_1733 * -0.005574649665504694f)));
                      float _1759 = min(max(mad(-0.23642469942569733f, _1746, mad(-0.32480329275131226f, _1743, (_1740 * 1.6410233974456787f))), 0.0f), 1.0f);
                      float _1760 = min(max(mad(0.016756348311901093f, _1746, mad(1.6153316497802734f, _1743, (_1740 * -0.663662850856781f))), 0.0f), 1.0f);
                      float _1761 = min(max(mad(0.9883948564529419f, _1746, mad(-0.008284442126750946f, _1743, (_1740 * 0.011721894145011902f))), 0.0f), 1.0f);
                      float _1764 = mad(0.15618768334388733f, _1761, mad(0.13400420546531677f, _1760, (_1759 * 0.6624541878700256f)));
                      float _1767 = mad(0.053689517080783844f, _1761, mad(0.6740817427635193f, _1760, (_1759 * 0.2722287178039551f)));
                      float _1770 = mad(1.0103391408920288f, _1761, mad(0.00406073359772563f, _1760, (_1759 * -0.005574649665504694f)));
                      float _1792 = min(max((min(max(mad(-0.23642469942569733f, _1770, mad(-0.32480329275131226f, _1767, (_1764 * 1.6410233974456787f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                      float _1793 = min(max((min(max(mad(0.016756348311901093f, _1770, mad(1.6153316497802734f, _1767, (_1764 * -0.663662850856781f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                      float _1794 = min(max((min(max(mad(0.9883948564529419f, _1770, mad(-0.008284442126750946f, _1767, (_1764 * 0.011721894145011902f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                      do {
                        if (!(cb0_040w == 5)) {
                          _1807 = mad(_45, _1794, mad(_44, _1793, (_1792 * _43)));
                          _1808 = mad(_48, _1794, mad(_47, _1793, (_1792 * _46)));
                          _1809 = mad(_51, _1794, mad(_50, _1793, (_1792 * _49)));
                        } else {
                          _1807 = _1792;
                          _1808 = _1793;
                          _1809 = _1794;
                        }
                        float _1819 = exp2(log2(_1807 * 9.999999747378752e-05f) * 0.1593017578125f);
                        float _1820 = exp2(log2(_1808 * 9.999999747378752e-05f) * 0.1593017578125f);
                        float _1821 = exp2(log2(_1809 * 9.999999747378752e-05f) * 0.1593017578125f);
                        _2562 = exp2(log2((1.0f / ((_1819 * 18.6875f) + 1.0f)) * ((_1819 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                        _2563 = exp2(log2((1.0f / ((_1820 * 18.6875f) + 1.0f)) * ((_1820 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                        _2564 = exp2(log2((1.0f / ((_1821 * 18.6875f) + 1.0f)) * ((_1821 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                      } while (false);
                    } while (false);
                  } while (false);
                } while (false);
              } while (false);
            } while (false);
          } while (false);
        } while (false);
      } else {
        if ((cb0_040w & -3) == 4) {
          _14[0] = cb0_010x;
          _14[1] = cb0_010y;
          _14[2] = cb0_010z;
          _14[3] = cb0_010w;
          _14[4] = cb0_012x;
          _14[5] = cb0_012x;
          _15[0] = cb0_011x;
          _15[1] = cb0_011y;
          _15[2] = cb0_011z;
          _15[3] = cb0_011w;
          _15[4] = cb0_012y;
          _15[5] = cb0_012y;
          float _1900 = cb0_012z * _1122;
          float _1901 = cb0_012z * _1123;
          float _1902 = cb0_012z * _1124;
          float _1905 = mad((UniformBufferConstants_WorkingColorSpace_256[0].z), _1902, mad((UniformBufferConstants_WorkingColorSpace_256[0].y), _1901, ((UniformBufferConstants_WorkingColorSpace_256[0].x) * _1900)));
          float _1908 = mad((UniformBufferConstants_WorkingColorSpace_256[1].z), _1902, mad((UniformBufferConstants_WorkingColorSpace_256[1].y), _1901, ((UniformBufferConstants_WorkingColorSpace_256[1].x) * _1900)));
          float _1911 = mad((UniformBufferConstants_WorkingColorSpace_256[2].z), _1902, mad((UniformBufferConstants_WorkingColorSpace_256[2].y), _1901, ((UniformBufferConstants_WorkingColorSpace_256[2].x) * _1900)));
          float _1915 = max(max(_1905, _1908), _1911);
          float _1920 = (max(_1915, 1.000000013351432e-10f) - max(min(min(_1905, _1908), _1911), 1.000000013351432e-10f)) / max(_1915, 0.009999999776482582f);
          float _1933 = ((_1908 + _1905) + _1911) + (sqrt((((_1911 - _1908) * _1911) + ((_1908 - _1905) * _1908)) + ((_1905 - _1911) * _1905)) * 1.75f);
          float _1934 = _1933 * 0.3333333432674408f;
          float _1935 = _1920 + -0.4000000059604645f;
          float _1936 = _1935 * 5.0f;
          float _1940 = max((1.0f - abs(_1935 * 2.5f)), 0.0f);
          float _1951 = ((float((int)(((int)(uint)((bool)(_1936 > 0.0f))) - ((int)(uint)((bool)(_1936 < 0.0f))))) * (1.0f - (_1940 * _1940))) + 1.0f) * 0.02500000037252903f;
          do {
            if (!(_1934 <= 0.0533333346247673f)) {
              if (!(_1934 >= 0.1599999964237213f)) {
                _1960 = (((0.23999999463558197f / _1933) + -0.5f) * _1951);
              } else {
                _1960 = 0.0f;
              }
            } else {
              _1960 = _1951;
            }
            float _1961 = _1960 + 1.0f;
            float _1962 = _1961 * _1905;
            float _1963 = _1961 * _1908;
            float _1964 = _1961 * _1911;
            do {
              if (!((bool)(_1962 == _1963) && (bool)(_1963 == _1964))) {
                float _1971 = ((_1962 * 2.0f) - _1963) - _1964;
                float _1974 = ((_1908 - _1911) * 1.7320507764816284f) * _1961;
                float _1976 = atan(_1974 / _1971);
                bool _1979 = (_1971 < 0.0f);
                bool _1980 = (_1971 == 0.0f);
                bool _1981 = (_1974 >= 0.0f);
                bool _1982 = (_1974 < 0.0f);
                float _1991 = select((_1981 && _1980), 90.0f, select((_1982 && _1980), -90.0f, (select((_1982 && _1979), (_1976 + -3.1415927410125732f), select((_1981 && _1979), (_1976 + 3.1415927410125732f), _1976)) * 57.2957763671875f)));
                if (_1991 < 0.0f) {
                  _1996 = (_1991 + 360.0f);
                } else {
                  _1996 = _1991;
                }
              } else {
                _1996 = 0.0f;
              }
              float _1998 = min(max(_1996, 0.0f), 360.0f);
              do {
                if (_1998 < -180.0f) {
                  _2007 = (_1998 + 360.0f);
                } else {
                  if (_1998 > 180.0f) {
                    _2007 = (_1998 + -360.0f);
                  } else {
                    _2007 = _1998;
                  }
                }
                do {
                  if ((bool)(_2007 > -67.5f) && (bool)(_2007 < 67.5f)) {
                    float _2013 = (_2007 + 67.5f) * 0.029629629105329514f;
                    int _2014 = int(_2013);
                    float _2016 = _2013 - float((int)(_2014));
                    float _2017 = _2016 * _2016;
                    float _2018 = _2017 * _2016;
                    if (_2014 == 3) {
                      _2046 = (((0.1666666716337204f - (_2016 * 0.5f)) + (_2017 * 0.5f)) - (_2018 * 0.1666666716337204f));
                    } else {
                      if (_2014 == 2) {
                        _2046 = ((0.6666666865348816f - _2017) + (_2018 * 0.5f));
                      } else {
                        if (_2014 == 1) {
                          _2046 = (((_2018 * -0.5f) + 0.1666666716337204f) + ((_2017 + _2016) * 0.5f));
                        } else {
                          _2046 = select((_2014 == 0), (_2018 * 0.1666666716337204f), 0.0f);
                        }
                      }
                    }
                  } else {
                    _2046 = 0.0f;
                  }
                  float _2055 = min(max(((((_1920 * 0.27000001072883606f) * (0.029999999329447746f - _1962)) * _2046) + _1962), 0.0f), 65535.0f);
                  float _2056 = min(max(_1963, 0.0f), 65535.0f);
                  float _2057 = min(max(_1964, 0.0f), 65535.0f);
                  float _2070 = min(max(mad(-0.21492856740951538f, _2057, mad(-0.2365107536315918f, _2056, (_2055 * 1.4514392614364624f))), 0.0f), 65504.0f);
                  float _2071 = min(max(mad(-0.09967592358589172f, _2057, mad(1.17622971534729f, _2056, (_2055 * -0.07655377686023712f))), 0.0f), 65504.0f);
                  float _2072 = min(max(mad(0.9977163076400757f, _2057, mad(-0.006032449658960104f, _2056, (_2055 * 0.008316148072481155f))), 0.0f), 65504.0f);
                  float _2073 = dot(float3(_2070, _2071, _2072), float3(0.2722287178039551f, 0.6740817427635193f, 0.053689517080783844f));
                  float _2084 = log2(max((lerp(_2073, _2070, 0.9599999785423279f)), 1.000000013351432e-10f));
                  float _2085 = _2084 * 0.3010300099849701f;
                  float _2086 = log2(cb0_008x);
                  float _2087 = _2086 * 0.3010300099849701f;
                  do {
                    if (!(!(_2085 <= _2087))) {
                      _2156 = (log2(cb0_008y) * 0.3010300099849701f);
                    } else {
                      float _2094 = log2(cb0_009x);
                      float _2095 = _2094 * 0.3010300099849701f;
                      if ((bool)(_2085 > _2087) && (bool)(_2085 < _2095)) {
                        float _2103 = ((_2084 - _2086) * 0.9030900001525879f) / ((_2094 - _2086) * 0.3010300099849701f);
                        int _2104 = int(_2103);
                        float _2106 = _2103 - float((int)(_2104));
                        float _2108 = _14[_2104];
                        float _2111 = _14[(_2104 + 1)];
                        float _2116 = _2108 * 0.5f;
                        _2156 = dot(float3((_2106 * _2106), _2106, 1.0f), float3(mad((_14[(_2104 + 2)]), 0.5f, mad(_2111, -1.0f, _2116)), (_2111 - _2108), mad(_2111, 0.5f, _2116)));
                      } else {
                        do {
                          if (!(!(_2085 >= _2095))) {
                            float _2125 = log2(cb0_008z);
                            if (_2085 < (_2125 * 0.3010300099849701f)) {
                              float _2133 = ((_2084 - _2094) * 0.9030900001525879f) / ((_2125 - _2094) * 0.3010300099849701f);
                              int _2134 = int(_2133);
                              float _2136 = _2133 - float((int)(_2134));
                              float _2138 = _15[_2134];
                              float _2141 = _15[(_2134 + 1)];
                              float _2146 = _2138 * 0.5f;
                              _2156 = dot(float3((_2136 * _2136), _2136, 1.0f), float3(mad((_15[(_2134 + 2)]), 0.5f, mad(_2141, -1.0f, _2146)), (_2141 - _2138), mad(_2141, 0.5f, _2146)));
                              break;
                            }
                          }
                          _2156 = (log2(cb0_008w) * 0.3010300099849701f);
                        } while (false);
                      }
                    }
                    float _2160 = log2(max((lerp(_2073, _2071, 0.9599999785423279f)), 1.000000013351432e-10f));
                    float _2161 = _2160 * 0.3010300099849701f;
                    do {
                      if (!(!(_2161 <= _2087))) {
                        _2230 = (log2(cb0_008y) * 0.3010300099849701f);
                      } else {
                        float _2168 = log2(cb0_009x);
                        float _2169 = _2168 * 0.3010300099849701f;
                        if ((bool)(_2161 > _2087) && (bool)(_2161 < _2169)) {
                          float _2177 = ((_2160 - _2086) * 0.9030900001525879f) / ((_2168 - _2086) * 0.3010300099849701f);
                          int _2178 = int(_2177);
                          float _2180 = _2177 - float((int)(_2178));
                          float _2182 = _14[_2178];
                          float _2185 = _14[(_2178 + 1)];
                          float _2190 = _2182 * 0.5f;
                          _2230 = dot(float3((_2180 * _2180), _2180, 1.0f), float3(mad((_14[(_2178 + 2)]), 0.5f, mad(_2185, -1.0f, _2190)), (_2185 - _2182), mad(_2185, 0.5f, _2190)));
                        } else {
                          do {
                            if (!(!(_2161 >= _2169))) {
                              float _2199 = log2(cb0_008z);
                              if (_2161 < (_2199 * 0.3010300099849701f)) {
                                float _2207 = ((_2160 - _2168) * 0.9030900001525879f) / ((_2199 - _2168) * 0.3010300099849701f);
                                int _2208 = int(_2207);
                                float _2210 = _2207 - float((int)(_2208));
                                float _2212 = _15[_2208];
                                float _2215 = _15[(_2208 + 1)];
                                float _2220 = _2212 * 0.5f;
                                _2230 = dot(float3((_2210 * _2210), _2210, 1.0f), float3(mad((_15[(_2208 + 2)]), 0.5f, mad(_2215, -1.0f, _2220)), (_2215 - _2212), mad(_2215, 0.5f, _2220)));
                                break;
                              }
                            }
                            _2230 = (log2(cb0_008w) * 0.3010300099849701f);
                          } while (false);
                        }
                      }
                      float _2234 = log2(max((lerp(_2073, _2072, 0.9599999785423279f)), 1.000000013351432e-10f));
                      float _2235 = _2234 * 0.3010300099849701f;
                      do {
                        if (!(!(_2235 <= _2087))) {
                          _2304 = (log2(cb0_008y) * 0.3010300099849701f);
                        } else {
                          float _2242 = log2(cb0_009x);
                          float _2243 = _2242 * 0.3010300099849701f;
                          if ((bool)(_2235 > _2087) && (bool)(_2235 < _2243)) {
                            float _2251 = ((_2234 - _2086) * 0.9030900001525879f) / ((_2242 - _2086) * 0.3010300099849701f);
                            int _2252 = int(_2251);
                            float _2254 = _2251 - float((int)(_2252));
                            float _2256 = _14[_2252];
                            float _2259 = _14[(_2252 + 1)];
                            float _2264 = _2256 * 0.5f;
                            _2304 = dot(float3((_2254 * _2254), _2254, 1.0f), float3(mad((_14[(_2252 + 2)]), 0.5f, mad(_2259, -1.0f, _2264)), (_2259 - _2256), mad(_2259, 0.5f, _2264)));
                          } else {
                            do {
                              if (!(!(_2235 >= _2243))) {
                                float _2273 = log2(cb0_008z);
                                if (_2235 < (_2273 * 0.3010300099849701f)) {
                                  float _2281 = ((_2234 - _2242) * 0.9030900001525879f) / ((_2273 - _2242) * 0.3010300099849701f);
                                  int _2282 = int(_2281);
                                  float _2284 = _2281 - float((int)(_2282));
                                  float _2286 = _15[_2282];
                                  float _2289 = _15[(_2282 + 1)];
                                  float _2294 = _2286 * 0.5f;
                                  _2304 = dot(float3((_2284 * _2284), _2284, 1.0f), float3(mad((_15[(_2282 + 2)]), 0.5f, mad(_2289, -1.0f, _2294)), (_2289 - _2286), mad(_2289, 0.5f, _2294)));
                                  break;
                                }
                              }
                              _2304 = (log2(cb0_008w) * 0.3010300099849701f);
                            } while (false);
                          }
                        }
                        float _2308 = cb0_008w - cb0_008y;
                        float _2309 = (exp2(_2156 * 3.321928024291992f) - cb0_008y) / _2308;
                        float _2311 = (exp2(_2230 * 3.321928024291992f) - cb0_008y) / _2308;
                        float _2313 = (exp2(_2304 * 3.321928024291992f) - cb0_008y) / _2308;
                        float _2316 = mad(0.15618768334388733f, _2313, mad(0.13400420546531677f, _2311, (_2309 * 0.6624541878700256f)));
                        float _2319 = mad(0.053689517080783844f, _2313, mad(0.6740817427635193f, _2311, (_2309 * 0.2722287178039551f)));
                        float _2322 = mad(1.0103391408920288f, _2313, mad(0.00406073359772563f, _2311, (_2309 * -0.005574649665504694f)));
                        float _2335 = min(max(mad(-0.23642469942569733f, _2322, mad(-0.32480329275131226f, _2319, (_2316 * 1.6410233974456787f))), 0.0f), 1.0f);
                        float _2336 = min(max(mad(0.016756348311901093f, _2322, mad(1.6153316497802734f, _2319, (_2316 * -0.663662850856781f))), 0.0f), 1.0f);
                        float _2337 = min(max(mad(0.9883948564529419f, _2322, mad(-0.008284442126750946f, _2319, (_2316 * 0.011721894145011902f))), 0.0f), 1.0f);
                        float _2340 = mad(0.15618768334388733f, _2337, mad(0.13400420546531677f, _2336, (_2335 * 0.6624541878700256f)));
                        float _2343 = mad(0.053689517080783844f, _2337, mad(0.6740817427635193f, _2336, (_2335 * 0.2722287178039551f)));
                        float _2346 = mad(1.0103391408920288f, _2337, mad(0.00406073359772563f, _2336, (_2335 * -0.005574649665504694f)));
                        float _2368 = min(max((min(max(mad(-0.23642469942569733f, _2346, mad(-0.32480329275131226f, _2343, (_2340 * 1.6410233974456787f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                        float _2369 = min(max((min(max(mad(0.016756348311901093f, _2346, mad(1.6153316497802734f, _2343, (_2340 * -0.663662850856781f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                        float _2370 = min(max((min(max(mad(0.9883948564529419f, _2346, mad(-0.008284442126750946f, _2343, (_2340 * 0.011721894145011902f))), 0.0f), 65535.0f) * cb0_008w), 0.0f), 65535.0f);
                        do {
                          if (!(cb0_040w == 6)) {
                            _2383 = mad(_45, _2370, mad(_44, _2369, (_2368 * _43)));
                            _2384 = mad(_48, _2370, mad(_47, _2369, (_2368 * _46)));
                            _2385 = mad(_51, _2370, mad(_50, _2369, (_2368 * _49)));
                          } else {
                            _2383 = _2368;
                            _2384 = _2369;
                            _2385 = _2370;
                          }
                          float _2395 = exp2(log2(_2383 * 9.999999747378752e-05f) * 0.1593017578125f);
                          float _2396 = exp2(log2(_2384 * 9.999999747378752e-05f) * 0.1593017578125f);
                          float _2397 = exp2(log2(_2385 * 9.999999747378752e-05f) * 0.1593017578125f);
                          _2562 = exp2(log2((1.0f / ((_2395 * 18.6875f) + 1.0f)) * ((_2395 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                          _2563 = exp2(log2((1.0f / ((_2396 * 18.6875f) + 1.0f)) * ((_2396 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                          _2564 = exp2(log2((1.0f / ((_2397 * 18.6875f) + 1.0f)) * ((_2397 * 18.8515625f) + 0.8359375f)) * 78.84375f);
                        } while (false);
                      } while (false);
                    } while (false);
                  } while (false);
                } while (false);
              } while (false);
            } while (false);
          } while (false);
        } else {
          if (cb0_040w == 7) {
            float _2442 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _1124, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _1123, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _1122)));
            float _2445 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _1124, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _1123, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _1122)));
            float _2448 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _1124, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _1123, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _1122)));
            float _2467 = exp2(log2(mad(_45, _2448, mad(_44, _2445, (_2442 * _43))) * 9.999999747378752e-05f) * 0.1593017578125f);
            float _2468 = exp2(log2(mad(_48, _2448, mad(_47, _2445, (_2442 * _46))) * 9.999999747378752e-05f) * 0.1593017578125f);
            float _2469 = exp2(log2(mad(_51, _2448, mad(_50, _2445, (_2442 * _49))) * 9.999999747378752e-05f) * 0.1593017578125f);
            _2562 = exp2(log2((1.0f / ((_2467 * 18.6875f) + 1.0f)) * ((_2467 * 18.8515625f) + 0.8359375f)) * 78.84375f);
            _2563 = exp2(log2((1.0f / ((_2468 * 18.6875f) + 1.0f)) * ((_2468 * 18.8515625f) + 0.8359375f)) * 78.84375f);
            _2564 = exp2(log2((1.0f / ((_2469 * 18.6875f) + 1.0f)) * ((_2469 * 18.8515625f) + 0.8359375f)) * 78.84375f);
          } else {
            if (!(cb0_040w == 8)) {
              if (cb0_040w == 9) {
                float _2516 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _1112, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _1111, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _1110)));
                float _2519 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _1112, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _1111, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _1110)));
                float _2522 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _1112, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _1111, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _1110)));
                _2562 = mad(_45, _2522, mad(_44, _2519, (_2516 * _43)));
                _2563 = mad(_48, _2522, mad(_47, _2519, (_2516 * _46)));
                _2564 = mad(_51, _2522, mad(_50, _2519, (_2516 * _49)));
              } else {
                float _2535 = mad((UniformBufferConstants_WorkingColorSpace_128[0].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[0].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[0].x) * _1136)));
                float _2538 = mad((UniformBufferConstants_WorkingColorSpace_128[1].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[1].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[1].x) * _1136)));
                float _2541 = mad((UniformBufferConstants_WorkingColorSpace_128[2].z), _1138, mad((UniformBufferConstants_WorkingColorSpace_128[2].y), _1137, ((UniformBufferConstants_WorkingColorSpace_128[2].x) * _1136)));
                _2562 = exp2(log2(mad(_45, _2541, mad(_44, _2538, (_2535 * _43)))) * cb0_040z);
                _2563 = exp2(log2(mad(_48, _2541, mad(_47, _2538, (_2535 * _46)))) * cb0_040z);
                _2564 = exp2(log2(mad(_51, _2541, mad(_50, _2538, (_2535 * _49)))) * cb0_040z);
              }
            } else {
              _2562 = _1122;
              _2563 = _1123;
              _2564 = _1124;
            }
          }
        }
      }
    }
  }
  SV_Target.x = (_2562 * 0.9523810148239136f);
  SV_Target.y = (_2563 * 0.9523810148239136f);
  SV_Target.z = (_2564 * 0.9523810148239136f);
  SV_Target.w = 0.0f;

  SV_Target = saturate(SV_Target);

  return SV_Target;
}
