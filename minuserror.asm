        Jump         $$main                    
        DLabel       $eat-location-zero        
        DataZ        8                         
        DLabel       $print-format-integer     
        DataC        37                        %% "%d"
        DataC        100                       
        DataC        0                         
        DLabel       $print-format-floating    
        DataC        37                        %% "%g"
        DataC        103                       
        DataC        0                         
        DLabel       $print-format-boolean     
        DataC        37                        %% "%s"
        DataC        115                       
        DataC        0                         
        DLabel       $print-format-newline     
        DataC        10                        %% "\n"
        DataC        0                         
        DLabel       $boolean-true-string      
        DataC        116                       %% "true"
        DataC        114                       
        DataC        117                       
        DataC        101                       
        DataC        0                         
        DLabel       $boolean-false-string     
        DataC        102                       %% "false"
        DataC        97                        
        DataC        108                       
        DataC        115                       
        DataC        101                       
        DataC        0                         
        DLabel       $print-spacer-string      
        DataC        32                        %% " "
        DataC        0                         
        DLabel       $usable-memory-start      
        DLabel       $global-memory-block      
        DataZ        25                        
        Label        $$main                    
        PushD        $global-memory-block      
        PushI        0                         
        Add                                    %% a
        PushI        1                         
        StoreI                                 
        Label        -whileStatement-statement-2 
        PushD        $global-memory-block      
        PushI        0                         
        Add                                    %% a
        PushD        $global-memory-block      
        PushI        0                         
        Add                                    %% a
        LoadI                                  
        PushI        1                         
        Add                                    
        StoreI                                 
        PushD        $global-memory-block      
        PushI        0                         
        Add                                    %% a
        LoadI                                  
        PushD        $print-format-integer     
        Printf                                 
        PushD        $print-format-newline     
        Printf                                 
        Label        -whileStatement-condition-2 
        Label        -compare-arg1-1           
        PushD        $global-memory-block      
        PushI        0                         
        Add                                    %% a
        LoadI                                  
        Label        -compare-arg2-1           
        PushI        10                        
        Label        -compare-sub-1            
        Subtract                               
        JumpNeg      -compare-true-1           
        Jump         -compare-false-1          
        Label        -compare-true-1           
        PushI        1                         
        Jump         -compare-join-1           
        Label        -compare-false-1          
        PushI        0                         
        Jump         -compare-join-1           
        Label        -compare-join-1           
        Dump
        JumpPos      -whileStatement-statement-2 
        Label        -whileStatement-exit-2    
        PushD        $global-memory-block      
        PushI        4                         
        Add                                    %% b
        PushI        1                         
        StoreI                                 
        Jump         -whileStatement-condition-5 
        Label        -whileStatement-statement-5 
        PushD        $global-memory-block      
        PushI        4                         
        Add                                    %% b
        PushD        $global-memory-block      
        PushI        4                         
        Add                                    %% b
        LoadI                                  
        PushI        1                         
        Add                                    
        StoreI                                 
        PushD        $global-memory-block      
        PushI        8                         
        Add                                    %% a
        PushI        1                         
        BNegate                                
        StoreC                                 
        PushD        $global-memory-block      
        PushI        9                         
        Add                                    %% c
        PushD        $global-memory-block      
        PushI        4                         
        Add                                    %% b
        LoadI                                  
        Negate                                 
        ConvertF                               
        StoreF                                 
        PushD        $global-memory-block      
        PushI        17                        
        Add                                    %% d
        PushD        $global-memory-block      
        PushI        9                         
        Add                                    %% c
        LoadF                                  
        PushF        0.010000                  
        FSubtract                              
        StoreF                                 
        PushD        $global-memory-block      
        PushI        17                        
        Add                                    %% d
        LoadF                                  
        PushD        $print-format-floating    
        Printf                                 
        PushD        $print-spacer-string      
        Printf                                 
        PushD        $global-memory-block      
        PushI        8                         
        Add                                    %% a
        LoadC                                  
        JumpTrue     -print-boolean-true4      
        PushD        $boolean-false-string     
        Jump         -print-boolean-join4      
        Label        -print-boolean-true4      
        PushD        $boolean-true-string      
        Label        -print-boolean-join4      
        PushD        $print-format-boolean     
        Printf                                 
        PushD        $print-format-newline     
        Printf                                 
        Label        -whileStatement-condition-5 
        Label        -compare-arg1-3           
        PushD        $global-memory-block      
        PushI        4                         
        Add                                    %% b
        LoadI                                  
        Label        -compare-arg2-3           
        PushI        10                        
        Label        -compare-sub-3            
        Subtract                               
        JumpNeg      -compare-true-3           
        Jump         -compare-false-3          
        Label        -compare-true-3           
        PushI        1                         
        Jump         -compare-join-3           
        Label        -compare-false-3          
        PushI        0                         
        Jump         -compare-join-3           
        Label        -compare-join-3           
        JumpPos      -whileStatement-statement-5 
        Label        -whileStatement-exit-5    
        Halt                                   
