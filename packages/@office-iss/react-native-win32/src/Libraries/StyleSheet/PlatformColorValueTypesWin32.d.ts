/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

// Use ColorValue typings when available in 0.63
// import {ColorValue} from 'react-native';

/**
 * A color stop within a gradient.  The values can either be other color values such as 'red', or a value thats already gone through processColor
 * Note: Currently we only support color stops from 0 to 1. (Where 0 is the gradient start, 1 is gradient end)
 */
export type GradientColorStop = {
  color: string /*ColorValue*/ | number /*ProcessedColorValue*/;
  offset: 0 | 1;
};

/**
 * Specifies the direction the gradient is in, and a list of color stops for the gradient.
 * Note: Currently we only support 2 color stops
 */
export type GradientColorValueWin32 = {
  gradientDirection: 'ToBottom' | 'ToTop' | 'ToRight' | 'ToLeft';
  colorStops: GradientColorStop[];
};

/**
 * Creates a color value that renders using a grandient brush.
 * This can be used to provide a value to most components that take a standard color value.
 */
export declare const ColorGradientWin32: (
    gradientColor: GradientColorValueWin32,
  ) => any /*ColorValue*/;
