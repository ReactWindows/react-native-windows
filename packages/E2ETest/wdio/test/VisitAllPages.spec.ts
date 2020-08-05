/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, By } from '../pages/BasePage';
//import HomePage from '../pages/HomePage';

//import assert from 'assert';

let pages = [
  '<ActivityIndicator>',
  '<Button>',
  '<CheckBox>',
  //  'Custom Views',
  '<DatePicker>',
  'Fast Path Texts',
  '<FlatList>',
  '<Flyout>',
  '<Glyph> UWP',
  '<Image>',
  //  '<FlatList> - MultiColumn',
  'New App Screen',
  '<Picker>',
  '<PickerWindows>',
  '<Pressable>',
  '<Popup>',
  'Keyboard extension Example',
  '<ScrollViewSimpleExample>',
  //  '<SectionList>',
  '<Switch>',
  '<Text>',
  '<Touchable*> and onPress',
  '<TransferProperties>',
  '<TransparentHitTestExample>',
  '<View>',
  'Keyboard Focus Example',
  //  'Accessibility',
  'Accessibility Windows',
  'AsyncStorage Windows',
  'Alert',
  'Animated - Examples',
  'Animated - Gratuitous App',
  'Appearance',
  'AppState',
  'Border',
  'Box Shadow',
  'Clipboard',
  'Crash',
  'DevSettings',
  'Dimensions',
  'Keyboard',
  'Layout Events',
  'Linking',
  'Layout - Flexbox',
  'Native Animated Example',
  'PanResponder Sample',
  'PlatformColor',
  'Pointer Events',
  'RTLExample',
  'Share',
  'Timers',
  'AppTheme',
  'WebSocket',
  'Transforms',
  //  '<LegacyControlStyleTest>',
  //  '<LegacyTransformTest>',
  //  '<LegacyTextInputTest>',
  //  '<LegacyLoginTest>',
  //  '<LegacyDirectManipulationTest>',
  //  '<LegacyImageTest>',
  //  '<LegacyAccessibilityTest>',
];

class TestPage extends BasePage {
  goToTestPage(page: string) {
    By(page).click();
    this.waitForPageLoaded();
  }
  backToHomePage() {
    this.homeButton.click();
  }
}

describe('VisitAllPagesTest', () => {
  pages.forEach(function(page) {
    it(page, () => {
      console.log('loading page ' + page);
      let testPage = new TestPage();
      testPage.goToTestPage(page);
      testPage.backToHomePage();
    });
  });
});
