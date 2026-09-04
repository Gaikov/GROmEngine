### Textures
- loading from pack
- mipmap generations
- rgb, rgba

### Common
- lighting
- matricies

### GLES 3
- current color
- texture matrix

### Vertex buffers
- Remove per-instance GPU vertex buffers from `nsSprite9SliceDesc`. Opening a popup currently creates a new `nsQuadsBuffer`/`GLVertexBuffer` for its `$9slice` background and destroys it again when the popup closes.
- Keep the calculated nine-slice geometry cached per descriptor and update it only when texture, atlas region, grid, width, or height changes.
- Draw cached geometry through a shared dynamic quad buffer owned by the renderer/engine, and release that buffer before the render device shuts down.
- Support multiple visible nine-slice sprites without rebuilding their geometry every frame. Longer term, batch compatible sprites that use the same texture and render state.
- Verify that repeatedly opening and closing all popup types no longer logs `initializing GL vertex buffer`, does not grow the allocated vertex-buffer count, and renders multiple nine-slice sprites correctly.
