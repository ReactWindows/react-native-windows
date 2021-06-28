/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Visit an example on the RNTester Components tab
 */
export async function goToComponentExample(example: string) {
  const componentsTabButton = await $('~components-tab');
  await componentsTabButton.click();
  await goToExample(example);
}

/**
 * Visit an example on the RNTester APIs tab
 */
export async function goToApiExample(example: string) {
  const componentsTabButton = await $('~apis-tab');
  await componentsTabButton.click();
  await goToExample(example);
}

async function goToExample(example: string) {
  // Filter the list down to the one test, to improve the stability of selectors
  const searchBox = await $('~explorer_search');
  await searchBox.setValue(regexEscape(example));

  const exampleButton = await $(`~${example}`);
  await exampleButton.click();

  // Make sure we've launched the example by waiting until the search box is
  // no longer present, but make sure we haven't crashed by checking that nav
  // buttons are still visible
  await browser.waitUntil(async () => !(await exampleButton.isDisplayed()));
  const componentsTab = await $('~components-tab');
  expect(await componentsTab.isDisplayed()).toBe(true);
}

function regexEscape(str: string): string {
  return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}
