<?php

/****************************************************************************************************
 *              ADMIN (form, validate, submit and ajax)
 ****************************************************************************************************/
function passpath_admin_form($form, &$form_state) {
  $form['#theme'] = 'passpath_admin_form';
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="passpath-form-wrapper">';
  $form['#suffix'] = '</div>';
  $form['help'] = array('#markup' => t("Add/Edit/Remove paths to pages that will be protected with password."));
  $paths = variable_get('passpath_protected_paths', array());
  $keys = array_keys($paths);
  // get current number of headlines
  $num = count($paths) ? count($paths) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["path"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($keys[$i]) ? $paths[$keys[$i]]['path'] : '',
      '#description' => t('An internal drupal path.'),
      '#autocomplete_path' => 'passpath/path/autocomplete',
      '#size' => 60,
      '#maxlength' => 200,
      '#field_prefix' => url('', array('absolute' => TRUE)),
    );
    $element["pass"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($keys[$i]) ? $paths[$keys[$i]]['pass'] : '',
      '#description' => t('Passphrase to access this path.'),
      '#size' => 40,
      '#maxlength' => 64,
    );
    $element["expire"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($keys[$i]) ? $paths[$keys[$i]]['expire'] : 0,
      '#suffix' => '<div class="description">'.t("Amount of time visitor will be able to access the page wihtout re-entering the password. Set it to 0 for never. Use fractions for minutes i.e.: 0.25 equals 15 minutes.").'</div>',
      '#field_suffix' => t('hour(s)'),
      '#size' => 3,
      '#maxlength' => 3,
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($keys[$i]) ? $paths[$keys[$i]]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'passpath_admin_form_ajax',
        'wrapper' => 'passpath-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'passpath_admin_form_ajax',
      'wrapper' => 'passpath-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'passpath_admin_form_ajax',
      'wrapper' => 'passpath-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function passpath_admin_form_validate($form, &$form_state) {
  // remove
  // if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    // if (count($_POST['tabs']) < 2) {
      // variable_del('passpath_protected_paths');
      // form_set_error('',"Last item has been removed.");
    // }
  // }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $var = variable_get('passpath_protected_paths', array());
    $keys = $paths = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['path'])) {
          form_error($form['tabs'][$key]['tabtitle'], t("Path is required."));
        }
        if (!valid_url($value['path'])) {
          form_error($form['tabs'][$key]['path'], t("Invalid path."));
        }
        if (!isset($var[$value['path']]) && empty($value['pass'])) {
          form_error($form['tabs'][$key]['pass'], t("Password is required."));
        }
        if (!ctype_digit($value['expire'])) {
          form_error($form['tabs'][$key]['expire'], t("Enter a number of hours."));
        }
        // prevent duplicates
        foreach ($form_state['values']['tabs'] as $k => $v) {
          if ($k != $key && $v['path'] == $value['path']) {
            form_error($form['tabs'][$k]['path'], t("Duplicate entry."));
          }
        }
      }
    }
  }
}
function passpath_admin_form_submit($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      variable_del('passpath_protected_paths');
      drupal_set_message(t("Last item has been removed."));
    }
    else {
      $removed = $form_state['triggering_element']['#array_parents'][1];
      $form_state['storage']['remove'][] = $removed;
      drupal_set_message(t("Changes will not be stored until the form is Saved."),'warning');
    }
  }
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
    drupal_set_message(t("Changes will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[$value['path']] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_lgbt_external_links',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('passpath_protected_paths', $values);
    drupal_set_message("Values were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function passpath_admin_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  return $form;
}