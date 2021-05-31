/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
import Platform from 'react-native/Libraries/Utilities/Platform'; // #6210
const {DynamicColorIOS, PlatformColor, StyleSheet, Text, View} = ReactNative;

function PlatformColorsExample() {
  function createTable() {
    let colors = [
      {
        label: 'Window',
        color: PlatformColor('Window'),
      },
      {
        label: 'Background',
        color: PlatformColor('Background'),
      },
      {
        label: 'ActiveCaption',
        color: PlatformColor('ActiveCaption'),
      },
      {
        label: 'InactiveCaption',
        color: PlatformColor('InactiveCaption'),
      },
      {
        label: 'ButtonFace',
        color: PlatformColor('ButtonFace'),
      },
      {
        label: 'ButtonText',
        color: PlatformColor('ButtonText'),
      },
      {
        label: 'WindowText',
        color: PlatformColor('WindowText'),
      },
      {
        label: 'CaptionText',
        color: PlatformColor('CaptionText'),
      },
      {
        label: 'Highlight',
        color: PlatformColor('Highlight'),
      },
      {
        label: 'HighlightText',
        color: PlatformColor('HighlightText'),
      },
      {
        label: 'Hotlight',
        color: PlatformColor('Hotlight'),
      },
      {
        label: 'GrayText',
        color: PlatformColor('GrayText'),
      },
      {
        label: 'InactiveCaptionText',
        color: PlatformColor('InactiveCaptionText'),
      },
    ];

    let table = [];
    for (let color of colors) {
      table.push(
        <View style={styles.row} key={color.label}>
          <Text style={styles.labelCell}>{color.label}</Text>
          <View
            style={{
              ...styles.colorCell,
              backgroundColor: color.color,
            }}
          />
        </View>,
      );
    }
    return table;
  }

  return <View style={styles.column}>{createTable()}</View>;
}

function FallbackColorsExample() {
  let color = {};
  if (Platform.OS === 'win32') {
    color = {
      label: "PlatformColor('bogus', 'Hotlight', 'also bogus')",
      color: PlatformColor('bogus', 'Hotlight', 'also bogus'),
    };
  } else {
    color = {
      label: 'Unexpected Platform.OS: ' + Platform.OS,
      color: 'red',
    };
  }

  return (
    <View style={styles.column}>
      <View style={styles.row}>
        <Text style={styles.labelCell}>{color.label}</Text>
        <View
          style={{
            ...styles.colorCell,
            backgroundColor: color.color,
          }}
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  column: {flex: 1, flexDirection: 'column'},
  row: {flex: 0.75, flexDirection: 'row'},
  labelCell: {
    flex: 1,
    alignItems: 'stretch',
    ...Platform.select({
      ios: {color: PlatformColor('labelColor')},
      default: {color: 'black'},
    }),
  },
  colorCell: {flex: 0.25, alignItems: 'stretch'},
});

exports.title = 'PlatformColor';
exports.description =
  'Examples that show how PlatformColors may be used in an app.';
exports.examples = [
  {
    title: 'Platform Colors',
    render(): React.Element<any> {
      return <PlatformColorsExample />;
    },
  },
  {
    title: 'Fallback Colors',
    render(): React.Element<any> {
      return <FallbackColorsExample />;
    },
  },
];
