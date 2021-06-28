const path = require('path');
const resolveSync = require('resolve').sync;

module.exports = (request, options) => {
  return resolveSync(request, {
    ...options,
    pathFilter: (pkg, _path, relativePath) => {
      if (pkg.name === 'react-native') {
        const basePath = path.dirname(require.resolve(pkg.name));
        const localPath = path.join(__dirname, relativePath);
        return path.relative(basePath, localPath);
      } else {
        return relativePath;
      }
    } 
  });
}
