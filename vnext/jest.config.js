module.exports = {
  resolver: './jest-resolver.js',
  snapshotResolver: './jest-snapshot-resolver.js',
  transform: {
    '^.+\\.(bmp|gif|jpg|jpeg|mp4|png|psd|svg|webp)$':
      '<rootDir>/jest/assetFileTransformer.js',
    '.*': './jest/preprocessor.js',
  },
  transformIgnorePatterns: [],
  setupFiles: ['./jest/setup.js'],
  timers: 'fake',
  testRegex: '/__tests__/.*-test\\.js$',
  testPathIgnorePatterns: [
    // Only run the version of the tests that are part of the merged source output
    'src',
  ],
  haste: {
    defaultPlatform: 'windows',
    platforms: ['windows']
  },
  unmockedModulePathPatterns: [
    'react',
    'Libraries/Renderer',
    'promise',
    'source-map',
    'fastpath',
    'denodeify',
  ],
  testEnvironment: 'node',
  collectCoverageFrom: ['Libraries/**/*.js'],
  coveragePathIgnorePatterns: [
    '/__tests__/',
    '/vendor/',
    '<rootDir>/Libraries/react-native/',
  ],
};