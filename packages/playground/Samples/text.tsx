/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, StyleSheet, Text, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>Welcome to React Native!</Text>
        <Text style={styles.customFont}>{'\u2713 Custom fonts supported'}</Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#C5CCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  customFont: {
    fontFamily: 'CustomFont',
    fontSize: 20,
    textAlign: 'center',
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
