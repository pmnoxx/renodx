// Recreated from good.asm
// Resource declarations from assembly
struct u0_t {
  uint val;
};
RWStructuredBuffer<u0_t> u0 : register(u0);

Texture2DArray<float4> t1 : register(t1);
Texture2DArray<uint4> t2 : register(t2);

cbuffer cb0 : register(b0)
{
  float4 cb0[35];
}

// Group shared memory - raw 4 bytes = uint
groupshared uint g0;

[numthreads(64, 1, 1)]
void main(uint vThreadIDInGroupFlattened: SV_GroupIndex, uint3 vThreadGroupID: SV_GroupID, uint3 vThreadID: SV_DispatchThreadID) {
    // Initialize group shared memory
    if (vThreadIDInGroupFlattened == 0) {
        g0 = 0;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // Texture bounds from constant buffer
    uint2 texBounds = asuint(cb0[33].xy) - uint2(1, 1);
    uint result = asuint(cb0[34].x); // Default value
    
    // Process first sample (current thread)
    uint baseX = (vThreadGroupID.x << 4) | (vThreadIDInGroupFlattened & 0xF);
    uint offsetZ = vThreadIDInGroupFlattened >> 4;
    uint baseY = vThreadGroupID.y * 16 + offsetZ;
    uint2 coord = min(uint2(baseX, baseY), texBounds);
    uint3 texCoord = uint3(coord, vThreadID.z);
    
    uint mask = t2.Load(uint4(texCoord, 0)).y;
    if (mask & 2) {
        float value = t1.Load(uint4(texCoord, 0)).w;
        // value = saturate(value); // HOTFIX
        value = 0.f;

        // Decode the value using the same math as assembly
        float scaledValue = value * 7.96874952 + 0.00392156886;
        uint intValue = (uint)scaledValue;
        float adjustedValue = (float)intValue * -0.125490203 + value;
        float normalizedValue = saturate(adjustedValue * 8.225806);
        
        // Calculate flags based on exact assembly logic
        uint flags = 0;
        
        // Check conditions
        bool eq3 = (intValue == 3);
        bool eq1 = (intValue == 1);
        bool eq2 = (intValue == 2);
        bool eq4 = (intValue == 4);
        bool eq5 = (intValue == 5);
        bool normPos = (normalizedValue > 0.0);
        
        // Base value: 5 if (intValue==3 || intValue==1), else 1
        if (eq1 || eq3) {
            flags += 5;
        } else {
            flags += 1;
        }
        
        // Add 8 if intValue==2
        if (eq2) flags += 8;
        
        // Add 16 if intValue==4  
        if (eq4) flags += 16;
        
        // Add 32 if intValue==5
        if (eq5) flags += 32;
        
        // Add 64 if normalizedValue > 0
        if (normPos) flags += 64;
        
        result |= flags;
    }
    
    // Process remaining 3 samples (offsets +64, +128, +192)
    uint offsets[3] = {64, 128, 192};
    
    for (uint i = 0; i < 3; i++) {
        uint sampleThreadID = vThreadIDInGroupFlattened + offsets[i];
        uint sampleX = (vThreadGroupID.x << 4) | (sampleThreadID & 0xF);
        uint sampleOffsetZ = sampleThreadID >> 4;
        uint sampleY = vThreadGroupID.y * 16 + sampleOffsetZ;
        
        coord = min(uint2(sampleX, sampleY), texBounds);
        texCoord = uint3(coord, vThreadID.z);
        
        mask = t2.Load(uint4(texCoord, 0)).y;
        if (mask & 2) {
            float value = t1.Load(uint4(texCoord, 0)).w;
            
            float scaledValue = value * 7.96874952 + 0.00392156886;
            uint intValue = (uint)scaledValue;
            float adjustedValue = (float)intValue * -0.125490203 + value;
            float normalizedValue = saturate(adjustedValue * 8.225806);
            
            uint flags = 0;
            
            bool eq3 = (intValue == 3);
            bool eq1 = (intValue == 1);
            bool eq2 = (intValue == 2);
            bool eq4 = (intValue == 4);
            bool eq5 = (intValue == 5);
            bool normPos = (normalizedValue > 0.0);
            
            if (eq1 || eq3) {
                flags += 5;
            } else {
                flags += 1;
            }
            
            if (eq2) flags += 8;
            if (eq4) flags += 16;
            if (eq5) flags += 32;
            if (normPos) flags += 64;
            
            result |= flags;
        }
    }
    
    // Accumulate result using atomic OR
    InterlockedOr(g0, result);
    
    GroupMemoryBarrierWithGroupSync();
    
    // Thread 0 writes final result
    if (vThreadIDInGroupFlattened == 0) {
        uint2 gridSize = (asuint(cb0[33].xy) + 15) >> 4;
        uint bufferIndex = vThreadGroupID.y * gridSize.x + vThreadGroupID.x;
        bufferIndex = bufferIndex + gridSize.x * gridSize.y * vThreadID.z;
        
        uint groupResult = g0;
        uint existingValue = u0[bufferIndex].val;
        u0[bufferIndex].val = groupResult | existingValue;
    }
} 