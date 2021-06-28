/**
 * @packagedocumentation
 *
 * This package provides Windows specific components and provides JS implementations for some react-native primitives
 * Cross platform React-native primitives should be imported from 'react-native'
 * Windows specific components need to be imported from 'react-native-windows'
 *
 */

/*
   This file is used to provide the typings for this package.  The typings should only include the Windows specific types, even though
   the package actually exports the whole of the Windows implementation of react-native.

   NOTE: Concrete classes, objects etc that actually need to be exported from the package,
         need to also be added to index.windows.js
*/

export * from './Libraries/Components/Flyout/FlyoutProps';
export * from './Libraries/Components/Flyout/Flyout';
export * from './Libraries/Components/Glyph/Glyph';
export * from './Libraries/Components/Popup/PopupProps';
export * from './Libraries/Components/Popup/Popup';
export * from './Libraries/Components/Keyboard/KeyboardExt';
export * from './Libraries/Components/Keyboard/KeyboardExtProps';
export * from './Libraries/Components/View/ViewWindowsProps';
export * from './Libraries/Components/View/ViewWindows';
export * from './Libraries/AppTheme/AppTheme';
export * from './Libraries/AppTheme/AppThemeTypes';
