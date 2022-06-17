function avg_bpm = pulse_detector()

    bpm = [];
    data = readtable("data/hr.xlsx");
    y = data.Var1;

    hr_threshold = 700;
    samples_per_second = 100;
    seconds_per_minute = 60;
    
    previous_sample = 0;
    sample_counter = 0;
    peak_detected = 0;
    
    for i = 1:length(y)
       current_sample = y(i);
       if(current_sample - previous_sample) > hr_threshold && peak_detected == 0
           peak_detected = 1;
       elseif (current_sample - previous_sample) > hr_threshold && peak_detected == 1
           num_seconds_between_peaks = sample_counter / samples_per_second;
           bpm_tmp = round(seconds_per_minute / num_seconds_between_peaks);
           if(bpm_tmp < 40 || bpm_tmp > 160)
               previous_sample = 0;
               sample_counter = 0;
           else
               bpm(end+1) = bpm_tmp;
           end
           sample_counter = 0;
       end
       if peak_detected == 1
           sample_counter = sample_counter + 1;
       end
       previous_sample = current_sample;
    end
    avg_bpm = mean(bpm);
end