import fs from 'fs'
import path from 'path';
import { copyAndReplace } from '../generator-common';

export const templateRoot = path.resolve('../../../vnext/template');

export const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

export async function tryMkdir(dir: string): Promise<void> {
  try {
    await fs.promises.mkdir(dir, {recursive: true});
  } catch (err) {}
}

export async function ensureWinUI3Project(folder: string) {
  const windowsDir = path.join(folder, 'windows');
  if (fs.existsSync(windowsDir)) {
    await fs.promises.rmdir(windowsDir, {recursive: true});
  }
  await tryMkdir(windowsDir);

  const replacements = {
    name: 'WithWinUI3',
    namespace: 'WithWinUI3',
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
    useWinUI3: false,
    useHermes: false,
    packagesConfigCppNugetPackages: [
      {
        id: 'Microsoft.ReactNative.Cxx',
        version: '1.0.0',
        hasProps: false,
        hasTargets: true,
      },
      {
        id: 'Microsoft.UI.Xaml',
        version: '2.3.4.5',
        hasProps: false,
        hasTargets: false,
      },
      {
        id: 'Microsoft.ProjectReunion.WinUI',
        version: '3.2.1.0',
        hasProps: false,
        hasTargets: false,
      },
    ],
  };

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.sln'),
    path.join(windowsDir, 'WithWinUI3.sln'),
    replacements,
    null
  );

  const projDir = path.join(windowsDir, 'WithWinUI3');
  await tryMkdir(projDir);

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'),
    path.join(projDir, 'WithWinUI3.vcxproj'),
    replacements,
    null
  );

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/packages.config'),
    path.join(projDir, 'packages.config'),
    replacements,
    null
  );

  await copyAndReplace(
    path.join(templateRoot, 'shared-app/proj/ExperimentalFeatures.props'),
    path.join(windowsDir, 'ExperimentalFeatures.props'),
    replacements,
    null
  );
}
