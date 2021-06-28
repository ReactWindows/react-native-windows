const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpApp.sln',
  project: {
    projectFile: 'SimpleCSharpApp\\SimpleCSharpApp.csproj',
  },
};

const expectedConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCSharpApp.sln',
  project: {
    projectFile: 'SimpleCSharpApp\\SimpleCSharpApp.csproj',
    projectName: 'SimpleCSharpApp',
    projectLang: 'cs',
    projectGuid: '{416476D5-974A-4EE2-8145-4E331297247E}',
  },
};

module.exports = {
  project: {
    windows: projectConfig,
  },
  dependency: {
    platforms: {
      windows: projectConfig,
    }
  },
  expectedConfig: expectedConfig,
  expectedConfigIgnoringOverride: expectedConfig,
};
