const path = require('path');
const resolveSync = require('resolve').sync;

module.exports = (request, options) => {
  return resolveSync(request, {
    ...options,
    preserveSymlinks: false,
    pathFilter: (pkg, _path, relativePath) => {
      if (pkg.name === 'react-native') {
        const basePath = path.dirname(require.resolve(`${pkg.name}/package.json`));
        const rnwPath = path.dirname(require.resolve('react-native-windows/package.json'));
        return path.relative(basePath, path.join(rnwPath, relativePath));
      } else {
        return relativePath;
      }
    } 
  });
}
