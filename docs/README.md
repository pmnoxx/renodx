# RenoDX Documentation

## Overview

This directory contains comprehensive documentation for the RenoDX project, a DirectX game modification toolset built on Reshade's add-on system. The documentation is designed to help developers understand, modify, and extend the codebase effectively.

## Documentation Structure

### Core Documentation

#### 📋 [prompt.md](prompt.md)
**Essential Reading First** - This document specifies that ALL documentation must be read before making changes to the codebase. It outlines the required reading order and development workflow.

#### 🏗️ [ARCHITECTURE.md](ARCHITECTURE.md)
High-level system architecture and design principles. Explains:
- Core architecture components
- Data flow through the system
- Key design principles
- Component relationships
- Performance characteristics

#### 🔨 [BUILD_SYSTEM.md](BUILD_SYSTEM.md)
How to build and configure the project. Covers:
- Prerequisites and dependencies
- Build configuration
- Shader compilation pipeline
- Game-specific builds
- Troubleshooting

#### 🔌 [RESHADE_INTEGRATION.md](RESHADE_INTEGRATION.md)
Understanding Reshade add-on system integration. Details:
- Addon lifecycle
- DirectX hook points
- Resource injection system
- GUI integration
- Error handling

#### 🎨 [SHADER_SYSTEM.md](SHADER_SYSTEM.md)
HLSL shader processing and injection system. Explains:
- Shader pipeline
- Decompiler tools
- HLSL processing
- Injection techniques
- Performance optimization

#### 🎮 [GAME_INTEGRATION.md](GAME_INTEGRATION.md)
How game-specific addons work. Covers:
- Addon structure
- Implementation patterns
- Shader organization
- Configuration management
- Performance considerations

#### 📚 [API_REFERENCE.md](API_REFERENCE.md)
Core API documentation. Provides:
- Reshade integration API
- Device and resource management
- Command list API
- Shader management
- Utility functions
- Error handling

#### 📖 [SOURCE_CODE_READING_PLAN.md](SOURCE_CODE_READING_PLAN.md)
Comprehensive plan for reading the entire source code. Includes:
- 8-week systematic reading plan
- Note-taking methodology
- Analysis strategies
- Success criteria
- Risk mitigation

### Additional Documentation

#### 📝 [CONTRIBUTING.md](CONTRIBUTING.md)
Development workflow and guidelines (existing file).

## Quick Start Guide

### For New Developers

1. **Start Here**: Read [prompt.md](prompt.md) to understand the development workflow
2. **Understand Architecture**: Read [ARCHITECTURE.md](ARCHITECTURE.md) for high-level understanding
3. **Set Up Build Environment**: Follow [BUILD_SYSTEM.md](BUILD_SYSTEM.md) to get the project building
4. **Learn Integration**: Study [RESHADE_INTEGRATION.md](RESHADE_INTEGRATION.md) to understand the foundation
5. **Explore Shaders**: Read [SHADER_SYSTEM.md](SHADER_SYSTEM.md) to understand shader processing
6. **Study Game Addons**: Review [GAME_INTEGRATION.md](GAME_INTEGRATION.md) for implementation patterns
7. **Reference API**: Use [API_REFERENCE.md](API_REFERENCE.md) for detailed API information

### For Experienced Developers

1. **Follow Reading Plan**: Use [SOURCE_CODE_READING_PLAN.md](SOURCE_CODE_READING_PLAN.md) for systematic code analysis
2. **Focus on Specific Areas**: Use targeted documentation for your area of interest
3. **Reference API**: Use [API_REFERENCE.md](API_REFERENCE.md) for implementation details

## Key Concepts

### RenoDX Architecture
RenoDX is built on Reshade's add-on system and provides:
- **DirectX Hooking**: Intercept DirectX API calls
- **Shader Injection**: Modify game shaders at runtime
- **Resource Management**: Inject custom textures and buffers
- **Game-Specific Addons**: Modular modifications per game

### Development Workflow
1. **Read Documentation**: Complete all required reading
2. **Understand Context**: Review relevant game implementations
3. **Plan Changes**: Document approach and impacts
4. **Implement**: Follow established patterns
5. **Test**: Ensure compatibility and performance
6. **Document**: Update relevant documentation

## Common Tasks

### Adding a New Game
1. Study [GAME_INTEGRATION.md](GAME_INTEGRATION.md) for patterns
2. Review existing game implementations in `src/games/`
3. Create game-specific directory and files
4. Implement addon using established patterns
5. Test with target game

### Modifying Shaders
1. Understand [SHADER_SYSTEM.md](SHADER_SYSTEM.md)
2. Use decompiler tools for analysis
3. Create custom HLSL shaders
4. Implement injection logic
5. Test performance impact

### Debugging Issues
1. Enable debug layers (see [API_REFERENCE.md](API_REFERENCE.md))
2. Use logging system for diagnostics
3. Profile performance with monitoring tools
4. Check compatibility with target game

## Resources

### External Documentation
- **HLSL Reference**: `windows-win32-direct3dhlsl.txt` (included)
- **DirectX Documentation**: Microsoft's DirectX documentation
- **Reshade Documentation**: Reshade's addon development guide

### Code Examples
- **Game Implementations**: `src/games/` directory
- **Utility Functions**: `src/utils/` directory
- **Shader Examples**: `src/shaders/` directory

### Tools
- **Decompiler**: `src/decompiler/` for shader analysis
- **Build Scripts**: `bd.ps1`, `bd32.ps1`, `build.cmd`
- **Development Tools**: `src/addons/devkit/`

## Contributing

### Documentation Standards
- Use clear, concise language
- Include code examples where appropriate
- Maintain cross-references between documents
- Update documentation when code changes

### Code Standards
- Follow established patterns in the codebase
- Use consistent naming conventions
- Include appropriate error handling
- Document complex algorithms and decisions

### Testing Requirements
- Test all changes with target games
- Verify performance impact
- Ensure compatibility with existing systems
- Validate error handling

## Support

### Getting Help
1. **Read Documentation**: Start with the documentation in this directory
2. **Review Examples**: Study existing implementations
3. **Check Issues**: Look for similar problems in the issue tracker
4. **Ask Community**: Use Discord or GitHub discussions

### Reporting Issues
When reporting issues, include:
- Game and version information
- RenoDX version and configuration
- Detailed error messages
- Steps to reproduce
- System information

## Version History

### Documentation Updates
- **v1.0**: Initial comprehensive documentation set
- **Future**: Regular updates as codebase evolves

### Compatibility
- **Reshade 5.0+**: Full compatibility
- **DirectX 11/12**: Supported
- **Windows 10/11**: Primary target platform

---

**Remember**: Always read [prompt.md](prompt.md) first before making any changes to the codebase!







