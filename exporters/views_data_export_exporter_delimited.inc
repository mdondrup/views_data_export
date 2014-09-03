<?php

require_once 'views_data_export_exporter.inc';

/**
 * Webform exporter for creating CSV/TSV delimited files.
 */
class ViewsDataExportExporterDelimited extends ViewsDataExportExporter {
  public $separator;

  function __construct($options) {
    $this->options = $options;

    $this->separator = isset($options['separator']) ? $options['separator'] : ',';
    // Convert tabs.
    if ($this->separator == '\t') {
      $this->separator = "\t";
    }
    $options['separator'] = $this->separator;

    parent::__construct($options);
  }

  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {

    $options['separator'] = array(
      'default' => ',',
      'translatable' => TRUE,
    );
    $options['quote'] = array(
      'default' => TRUE,
      'translatable' => TRUE,
    );
    $options['trim'] = array(
      'default' => FALSE,
      'translatable' => FALSE,
    );
    $options['replace_newlines'] = array(
      'default' => FALSE,
      'translatable' => FALSE,
    );
    $options['newline_replacement'] = array(
      'default' => ', ',
      'translatable' => FALSE,
    );
    $options['header'] = array(
      'default' => TRUE,
      'translatable' => FALSE,
    );
    $options['keep_html'] = array(
      'default' => FALSE,
      'translatable' => FALSE,
    );
    $options['encoding'] = array(
      'default' => '',
      'translatable' => FALSE,
    );

    return $options;
  }

  /**
   * Options form mini callback.
   *
   * @param $form
   * Form array to add additional fields to.
   * @param $form_state
   * State of the form.
   * @return
   * None.
   */
  function options_form(&$form, &$form_state, $field_labels) {

    $form['separator'] = array(
      '#type' => 'textfield',
      '#title' => t('Separator'),
      '#default_value' => !empty($this->options['separator']) ? $this->options['separator'] : ',',
      '#description' => t('This is the separator that is used to separate fields. CSV implies comma separated fields so this should not be changed unless you have specific requirements'),
    );
    $form['quote'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['quote']),
      '#title' => t('Quote values. Useful for output that might contain your separator as part of one of the values.'),
    );
    $form['trim'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['trim']),
      '#title' => t('Trim whitespace from rendered fields. Can be useful for some themes where output results in extra newlines.'),
    );
    $form['replace_newlines'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['replace_newlines']),
      '#title' => t('Replace newlines in rendered fields.'),
    );
    $form['newline_replacement'] = array(
      '#prefix' => '<div><div id="edit-options-newline-replacement">',
      '#suffix' => '</div></div>',
      '#type' => 'textfield',
      '#title' => t('Replacement'),
      '#default_value' => $this->options['newline_replacement'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-style-options-replace-newlines' => array(TRUE)),
    );
    $form['header'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make first row a list of column headers.'),
      '#default_value' => !empty($this->options['header']),
    );
    $form['keep_html'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['keep_html']),
      '#title' => t('Keep HTML tags.'),
    );
    $form['encoding'] = array(
      '#type' => 'textfield',
      '#default_value' => !empty($this->options['encoding']) ? $this->options['encoding'] : '',
      '#title' => t('Character encoding conversion'),
      '#description' => t('Optionally specify a character conversion that some CSV file readers need. Use "utf8_decode" for ISO-8859-1 via <a href="@utf8_decode">utf8_decode PHP function</a>, other values will be passed <a href="@iconv">iconv</a> (this requires the iconv PHP extension), empty or illegal values will leave the output as UTF-8.',
        array('@iconv' => 'http://www.php.net/manual/en/function.iconv.php', '@utf8_decode' => 'http://www.php.net/manual/en/function.utf8-decode.php')),
    );
    return $form;
  }

  function add_row(&$file_handle, $data, $row_count) {
    foreach ($data as $key => $value) {
      // Escape inner quotes and wrap all contents in new quotes.
      $data[$key] = '"' . str_replace('"', '""', $data[$key]) . '"';

      // Remove <script> tags, which mysteriously cause Excel not to import.
      $data[$key] = preg_replace('!<(/?script.*?)>!', '[$1]', $data[$key]);
    }
    $row = implode($this->separator, $data) . "\n";

    @fwrite($file_handle, $row);
  }

  function add_header(&$file_handle, $filename) {
    return 'file header';
  }

  function get_headers($filename) {
    $headers = parent::get_headers($filename);

    // Convert tabs.
    if ($this->separator == "\t") {
      $extension = 'tsv';
      $content_type = 'text/tab-separated-values';
    }
    else {
      $extension = 'csv';
      $content_type = 'text/csv';
    }

    $headers['Content-Type'] = $content_type;
    $headers['Content-Disposition'] = "attachment; filename=$filename.$extension";
    return $headers;
  }

  /**
   * Alter the options on submission.
   */
  function options_submit() {

  }

  /**
   * Validate the options.
   */
  function options_validate(&$form, &$form_state) {

  }
}
