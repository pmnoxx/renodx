# RenoDX Development Guidelines

## IMPORTANT: Read All Documentation Before Making Changes

**Before making any changes to the RenoDX codebase, you MUST read and understand all documentation files in the `docs/` directory.**

### Required Reading Order:

1. **ARCHITECTURE.md** - High-level system architecture and design principles
2. **BUILD_SYSTEM.md** - How to build and configure the project
3. **RESHADE_INTEGRATION.md** - Understanding Reshade add-on system integration
4. **SHADER_SYSTEM.md** - HLSL shader processing and injection system
5. **GAME_INTEGRATION.md** - How game-specific addons work
6. **API_REFERENCE.md** - Core API documentation
7. **CONTRIBUTING.md** - Development workflow and guidelines

### Development Workflow:

1. **Read Documentation**: Complete all required reading above
2. **Understand Context**: Review relevant game-specific implementations in `src/games/`
3. **Plan Changes**: Document your approach and potential impacts
4. **Implement**: Follow coding standards and patterns established in the codebase
5. **Test**: Ensure compatibility with existing systems
6. **Document**: Update relevant documentation files

### Key Principles:

- **Reshade Compatibility**: All changes must maintain compatibility with Reshade's add-on system
- **Game Agnostic**: Core systems should remain game-agnostic
- **Performance**: Consider performance implications of all changes
- **Backward Compatibility**: Maintain compatibility with existing mods and configurations

### Getting Help:

- Review existing game implementations in `src/games/` for examples
- Check `src/utils/` for common utilities and patterns
- Refer to `external/reshade/` for Reshade integration details
- Use the decompiler tool (`src/decompiler/`) for shader analysis

---

**Remember**: This is a complex DirectX hooking and shader injection system. Understanding the full architecture is crucial for making safe and effective changes.





