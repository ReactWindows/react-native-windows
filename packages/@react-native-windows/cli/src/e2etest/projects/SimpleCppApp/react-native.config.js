const projectConfig = {
  sourceDir: 'windows',
  solutionFile: 'SimpleCppApp.sln',
  project: {
    projectFile: 'SimpleCppApp\\SimpleCppApp.vcxproj',
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
};
