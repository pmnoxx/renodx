// Recreated from good2.asm
// Resource declarations from assembly

// Structured buffer input
StructuredBuffer<uint> t0 : register(t0);

// UAV buffer for atomic operations
RWBuffer<uint> u0 : register(u0);

// UAV structured buffer output
RWStructuredBuffer<uint> u1 : register(u1);

// Constant buffers
cbuffer cb0 : register(b0)
{
    uint2 cb0_data; // cb0[0].xy
}

cbuffer cb1 : register(b1)
{
    float4 cb1[366];
}

// Immediate constant buffer data (29 entries) - corrected values from assembly
static const uint icb_data[29] = {
    0x00058001, 0x0005800d, 0x00058fff, 0x00015001, 0x00016001, 
    0x0005c001, 0x0005d001, 0x00058021, 0x0001500d, 0x0001600d,
    0x0005c00d, 0x0005d00d, 0x0005802d, 0x00015011, 0x00016011,
    0x0005c011, 0x0005d011, 0x00058031, 0x00015041, 0x00016041,
    0x0005c041, 0x0005d041, 0x00058081, 0x00015021, 0x00016021,
    0x0005c021, 0x0005d021, 0x00058041, 0x00058201
};

[numthreads(64, 1, 1)]
void main(uint3 vThreadID: SV_DispatchThreadID) {
    // Early exit check
    if (vThreadID.x >= cb0_data.x) {
        return;
    }
    
    // Calculate buffer index: vThreadID.z * cb0[0].x + vThreadID.x
    uint bufferIndex = vThreadID.z * cb0_data.x + vThreadID.x;
    
    // Load value from structured buffer t0
    uint loadedValue = t0[bufferIndex];
    
    // Check if (loadedValue & 4095) != 0
    uint maskedValue = loadedValue & 4095;
    if (maskedValue != 0) {
        // Calculate coordinates
        float threadX_f = (float)vThreadID.x + 0.5;
        float cb0_y_f = (float)cb0_data.y;
        uint coordY = (uint)(threadX_f / cb0_y_f);
        uint coordZ = vThreadID.x - (coordY * cb0_data.y);
        
        // Search through immediate constant buffer for matching pattern
        uint matchIndex = 28; // Default value if no match found
        bool found = false;
        
        for (uint i = 0; i < 29; i++) {
            // Check if (loadedValue & icb_data[i]) == loadedValue
            if ((loadedValue & icb_data[i]) == loadedValue) {
                matchIndex = i;
                found = true;
                break;
            }
        }
        
        // Calculate atomic buffer offset
        uint atomicOffset = matchIndex * 3;
        
        // Perform atomic add operation and get previous value
        uint previousValue;
        InterlockedAdd(u0[atomicOffset], 4, previousValue);
        
        // Extract the counter part (shift right by 2)
        uint counter = previousValue >> 2;
        
        // Pack the result value
        uint packedResult = 0;
        packedResult |= coordZ;                    // Lower bits
        packedResult |= (coordY << 15);           // Middle bits  
        packedResult |= (vThreadID.z << 30);      // Upper bits
        
        // Calculate final output index
        uint outputIndex = matchIndex * cb0_data.x;
        outputIndex = outputIndex * asuint(cb1[365].z) + counter;
        
        // Store result to structured buffer u1
        u1[outputIndex] = packedResult;
    }
} 