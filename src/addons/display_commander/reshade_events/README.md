# ReShade Events

This folder contains event handlers for various ReShade events that Display Commander needs to intercept.

## Files

- **fullscreen_prevention.hpp/cpp** - Handles `set_fullscreen_state` events to prevent applications from going fullscreen when the user has enabled fullscreen prevention

## Design Principles

- Keep event handlers simple and focused
- Return `true` to suppress the event, `false` to allow it to proceed
- No external checks or complex logic
- Direct integration with Display Commander settings

